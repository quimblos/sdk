import { Node } from './node';

export class Thread {
    public __wasm: wasm.Thread;

    private running = false;
    private waitTimeout?: any;

    public constructor(
        public node: Node,
        public name: string,
        public hex: string,
        public ports: wasm.PortDef[],
    ) {
        const port_defs = new wasm.VectorPortDef();
        for (const port of ports) {
            port_defs.push_back(port);
        }
        this.__wasm = new wasm.Thread(
            node.__wasm,
            name,
            hex,
            port_defs
        );
    }

    public get state(): keyof typeof wasm.ThreadState {
        const v = this.__wasm.get_state().value;
        return Object.entries(wasm.ThreadState).find(e => (e[1] as any)?.value === v)[0] as any;
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