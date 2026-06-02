import make_wasm_kernel, { Kernel } from './quimblos';
import { Driver } from './driver';
import { Node } from './node';

export class Engine {
    public __wasm: wasm.Engine;
    public kernel: Kernel;

    private drivers: Record<string, Driver> = {};
    private nodes: Record<string, Node> = {};

    // private logs: string[] = [];
    private topic = new EventTarget();

    // Constructor

    public static async init(): Promise<Engine> {
        const engine = new Engine();
        const kernel = await make_wasm_kernel({
            print: engine._log.bind(engine),
            printErr: engine._log.bind(engine)
        });

        kernel.VectorPortDef.from = (items) => {
            const vec = new kernel.VectorPortDef();
            for (const item of items) {
                vec.push_back(item);
            }
            return vec;
        }
        kernel.VectorTypeDef.from = (items) => {
            const vec = new kernel.VectorTypeDef();
            for (const item of items) {
                vec.push_back(item);
            }
            return vec;
        }

        engine.kernel = kernel;
        engine.__wasm = new kernel.Engine(new kernel.VectorPortDef());

        return engine;
    }

    // Drivers

    public link_driver(driver: Driver) {
        this.drivers[driver.name] = driver;
        return this.__wasm.link_driver(driver.__wasm);
    }
    public delete_driver(name: string) {
        delete this.drivers[name];
        return this.__wasm.delete_driver(name);
    }

    public link_node(node: Node) {
        this.nodes[node.name] = node;
        return this.__wasm.link_node(node.__wasm);
    }
    public delete_node(name: string) {
        delete this.nodes[name];
        return this.__wasm.delete_node(name);
    }

    private _log(msg: string) {
        console.log(msg);
        this.topic.dispatchEvent(new CustomEvent('log', {
            detail: msg
        }));
    }

    public onLog(fn: (msg: string) => void) {
        this.topic.addEventListener('log', (ev: CustomEvent) => fn(ev.detail));
    }

    public get_driver(name: string) {
        return this.drivers[name];
    }
    public get_driver_index(name: string) {
        return Object.keys(this.drivers).findIndex(k => k === name);
    }
    public get_drivers() {
        return Object.values(this.drivers);
    }
}