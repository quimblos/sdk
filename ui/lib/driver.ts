import { GooWebComponent } from '@quimblos/goo';
import { quimblos } from '@quimblos/compiler/src/lang/semantics';
import { Engine } from './engine';

export type DriverData = {
    [port: number]: {
        index: number,
        type: string,
        value: any
    }
}

export abstract class Driver<T = {}> {
    public __wasm: wasm.Driver;
    protected webc: Record<string, GooWebComponent & T & { name: string }> = {};

    public variables: quimblos.Identifier[] = []

    protected constructor(
        public name: string,
        public webc_name: string,
        public ports: wasm.PortDef[]
    ) {
        const port_defs = new wasm.VectorPortDef();
        for (const port of ports) {
            port_defs.push_back(port);

            const identifier = new quimblos.Identifier();
            identifier.name = port.name;

            identifier.type = new quimblos.TypeIdentifier();
            identifier.type.name = 'u8';
            identifier.type.arr_length = 0;
            this.variables.push(identifier);
        }
        this.__wasm = new wasm.Driver(
            name,
            port_defs
        );
        this.__wasm.bind(this);
    }

    public make_webc(name: string, parent: HTMLElement) {
        if (name in this.webc) return;
        this.webc[name] = (window as any).goo.make(this.webc_name, {});
        const webc = this.webc[name];
        if (webc.parentElement) {
            webc.parentElement.removeChild(webc);
        }
        parent.appendChild(webc);
        this.setup();
    }

    public destroy_webc(name: string) {
        if (!(name in this.webc)) return;
        const webc = this.webc[name];
        if (webc.parentElement) {
            webc.parentElement.removeChild(webc);
        }
        delete this.webc[name];
    }

    public abstract setup(): void;
    public abstract log(value: any): void;
    public abstract render(data: DriverData): void;
}