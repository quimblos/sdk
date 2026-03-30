import { GooWebComponent } from '@quimblos/goo';
import WASM, { Kernel } from './quimblos';
import { qbscript } from './grammar/semantics';


export class Engine {
    private static instance?: Engine;
    public __qb: wasm.Engine;

    private devices: Device[] = []
    private runners: Record<string, Runner> = {};

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

    public put_device(device: Device) {
        this.devices.push(device);
        this.__qb.put_device((device as any).__qb);
    }

    public make_runner(name: string, hex: string, remake = true) {
        if (remake) {
            this.delete_runner(name);
        }
        const runner = new Runner(this, name, hex);
        this.runners[name] = runner;
        return runner;
    }

    public delete_runner(name: string) {
        if (!(name in this.runners)) return;
        const runner = this.runners[name];
        this.__qb.delete_runner(name);
        delete this.runners[name];
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

    public get_device(name: string) {
        return this.devices.find(d => d.name === name);
    }
    public get_device_index(name: string) {
        return this.devices.findIndex(d => d.name === name);
    }
    public get_devices() {
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
        if (engine.__qb.make_runner(name, hex) != 0) {
            throw new Error('Failed to create runner');
        }
        this.__qb = engine.__qb.get_runner(name);
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

export abstract class Device<T = {}> {
    public __qb: wasm.Device;
    protected webc: Record<string, GooWebComponent & T> = {};

    public nodes: qbscript.Identifier[] = []

    protected constructor(
        public name: string,
        public webc_name: string,
        nodes: wasm.DeviceNode[]
    ) {
        const regs = new wasm.VectorDeviceNode();
        for (const node of nodes) {
            regs.push_back({
                name: node.name,
                type: node.type,
                arr_length: node.arr_length ?? 0
            });
                        
            const identifier = new qbscript.Identifier();
            identifier.name = node.name;
            identifier.type = new qbscript.TypeIdentifier();
            identifier.type.name = node.type;
            identifier.type.arr_length = node.arr_length;
            this.nodes.push(identifier);
        }
        this.__qb = new wasm.Device(name, regs);
        this.__qb.bind(this);
    }

    public makeWebc(name: string, parent: HTMLElement) {
        if (name in this.webc) return;
        this.webc[name] = (window as any).goo.make(this.webc_name, {});
        const webc = this.webc[name];
        if (webc.parentElement) {
            webc.parentElement.removeChild(webc);
        }
        parent.appendChild(webc);
        this.setup();
    }

    public destroyWebc(name: string) {
        if (!(name in this.webc)) return;
        const webc = this.webc[name];
        if (webc.parentElement) {
            webc.parentElement.removeChild(webc);
        }
        delete this.webc[name];
    }

    public abstract setup(): void;
    public abstract update(value: any): void;
}

