import { GooWebComponent } from '@quimblos/goo';
import WASM, { Kernel } from './quimblos';
import { quimblos } from '@quimblos/compiler/src/lang/semantics';


export class Engine {
    private static instance?: Engine;
    public __wasm: wasm.Engine;

    private devices: Device[] = []
    private runners: Record<string, Runner> = {};

    // private logs: string[] = [];
    private target = new EventTarget();

    private kernel: Kernel
    private boot(kernel: Kernel) {
        this.kernel = kernel;
        this.__wasm = new kernel.Engine();
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

        wasm.VectorCode.from = (items) => {
            const vec = new wasm.VectorCode();
            for (const item of items) {
                vec.push_back(item);
            }
            return vec;
        }
        wasm.VectorDeviceData.from = (items) => {
            const vec = new wasm.VectorDeviceData();
            for (const item of items) {
                vec.push_back(item);
            }
            return vec;
        }

        engine.boot(wasm);
        return engine;
    }

    public link_device(device: Device) {
        this.devices.push(device);
        this.__wasm.link_device((device as any).__wasm);
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
        this.__wasm.delete_runner(name);
        delete this.runners[name];
        return runner;
    }

    private _log(msg: string) {
        console.log(msg);
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
    public __wasm: wasm.Runner;
    private running = false;
    private waitTimeout?: any;

    public constructor(
        engine: Engine,
        public name: string,
        public hex: string
    ) {
        const res = engine.__wasm.make_runner(name, hex);
        if (!res.ok) {
            throw new Error(res.message);
        }
        this.__wasm = engine.__wasm.get_runner(name);
    }

    public get state(): keyof typeof wasm.RunnerState {
        const v = this.__wasm.get_state().value;
        return Object.entries(wasm.RunnerState).find(e => (e[1] as any)?.value === v)[0] as any;
    }

    public get sleep(): number {
        return this.__wasm.get_sleep();
    }

    public async run() {
        this.running = true;
        this.__wasm.start();
        while (this.__wasm.tick() && this.running) {
            if (this.state === 'SLEEPING') {
                await this.wait(this.sleep);
                this.__wasm.wakeup();
            }
        }
        this.running = false;
    }

    public stop() {
        this.running = false;
        this.__wasm.reset();
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

export type DeviceDataOut = Record<string, {
    index: number
    value: any
}>

export abstract class Device<T = {}> {
    public __wasm: wasm.Device;
    protected webc: Record<string, GooWebComponent & T> = {};

    public variables: quimblos.Identifier[] = []

    protected constructor(
        public name: string,
        public webc_name: string,
        nodes: wasm.DeviceData[]
    ) {
        const variables = new wasm.VectorDeviceData();
        for (const node of nodes) {
            variables.push_back(node);

            const identifier = new quimblos.Identifier();
            identifier.name = node.name;

            identifier.type = new quimblos.TypeIdentifier();
            identifier.type.name = 'u8';
            identifier.type.arr_length = 0;
            this.variables.push(identifier);
        }
        this.__wasm = wasm.Device.make(name, variables);
        this.__wasm.bind(this);
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
    public abstract log(value: DeviceDataOut[string]): void;
    public abstract on_tick(data: DeviceDataOut): void;
}

