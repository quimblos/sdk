import { GooWebComponent } from '@quimblos/goo';
import WASM, { Kernel } from './quimblos';


export class Engine {
    private static instance?: Engine;
    public __qb: wasm.Engine;

    private devices: Device[] = []
    private runners: Runner[] = []

    // private logs: string[] = [];
    private target = new EventTarget();

    private kernel: Kernel
    private boot(kernel: Kernel) {
        this.kernel = kernel;
        this.__qb = new kernel.Engine();
    }

    public static async init() {
        if (this.instance) {
            return this.instance;
        }
        const engine = new Engine();
        const wasm = await WASM({
            print: engine._log.bind(engine),
            printErr: engine._log.bind(engine)
        });

        (window as any).wasm = wasm;
        (window as any).qb = engine;

        engine.boot(wasm);
        return engine;
    }

    public putDevice(device: Device) {
        this.devices.push(device);
        this.__qb.putDevice((device as any).__qb);
    }

    public makeRunner(name: string, hex: string) {
        const runner = new Runner(this, name, hex);
        this.runners.push(runner);
        return runner;
    }

    private _log(msg: string) {
        this.target.dispatchEvent(new CustomEvent('log', {
            detail: msg
        }));
    }

    public onLog(fn: (msg: string) => void) {
        this.target.addEventListener('log', (ev: CustomEvent) => fn(ev.detail));
    }

    public getDevices() {
        return this.devices;
    }
}


export class Runner {
    public __qb: wasm.Runner;
    private running = false;
    private waitTimeout?: any;

    public constructor(
        engine: Engine,
        public name: string,
        public hex: string
    ) {
        if (engine.__qb.makeRunner(name, hex) != 0) {
            throw new Error('Failed to create runner');
        }
        this.__qb = engine.__qb.getRunner(name);
    }

    public get state(): keyof typeof wasm.RunnerState {
        const v = this.__qb.getState().value;
        return Object.entries(wasm.RunnerState).find(e => (e[1] as any)?.value === v)[0] as any;
    }

    public get sleep(): number {
        return this.__qb.getSleep();
    }

    public async run() {
        this.running = true;
        this.__qb.start();
        while (this.__qb.tick() && this.running) {
            if (this.state === 'SLEEPING') {
                await this.wait(this.sleep);
                this.__qb.wakeup();
            }
        }
        this.running = false;
    }

    public stop() {
        this.running = false;
        this.__qb.reset();
        if (this.waitTimeout) {
            clearTimeout(this.waitTimeout)
        }
    }

    private async wait(ms: number) {
        return new Promise<void>(resolve => {
            this.waitTimeout = setTimeout(resolve, ms);
        })
    }
}

export type DataType = 'void'|'u8'|'i8'|'b8'|'u16'|'i16'|'u32'|'i32'|'f32'|'str'|'const';

export abstract class Device<T = {}> {
    public __qb: wasm.Device;
    protected webc?: GooWebComponent & T;

    protected constructor(
        public name: string,
        public webc_name: string,
        public registers: wasm.DeviceRegister[]
    ) {
        const regs = new wasm.VectorDeviceRegister();
        for (const port of registers) {
            regs.push_back(port);
        }
        this.__qb = new wasm.Device(name, regs);
        this.__qb.bind(this);
    }

    public makeWebc(parent: HTMLElement) {
        this.webc ??= (window as any).goo.make(this.webc_name, {});
        if (this.webc.parentElement) {
            this.webc.parentElement.removeChild(this.webc);
        }
        parent.appendChild(this.webc);
        this.setup();
    }

    public abstract setup(): void;
    public abstract update(value: any): void;
}

