import { Engine } from './engine';
import { Thread } from './thread';

export class Node {
    public __wasm: wasm.Node;

    public constructor(
        public engine: Engine,
        public name: string,
        public ports: wasm.PortDef[],
    ) {
        const port_defs = new wasm.VectorPortDef();
        for (const port of ports) {
            port_defs.push_back(port);
        }
        this.__wasm = new wasm.Node(
            engine.__wasm,
            name,
            port_defs
        );
    }
    
    public link_thread(thread: Thread): boolean {
        return this.__wasm.link_thread(thread.__wasm);
    }
    
    public delete_thread(name: string): boolean {
        return this.__wasm.delete_thread(name);
    }
}