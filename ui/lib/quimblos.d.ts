import { quimblos } from "@quimblos/compiler/src/lang/semantics"
import { Engine } from "./engine"

declare global {
    namespace wasm {
        class VectorTypeDef {
            static from(items: TypeDef[]): VectorTypeDef;
            public push_back(val: TypeDef): void;    
        }
        class VectorPortDef {
            static from(items: PortDef[]): VectorPortDef;
            public push_back(val: PortDef): void;    
        }

        enum TypeKind {
            VOID = 0x00,    
            BOOL = 0x01,    
            INT = 0x02,     
            FLOAT = 0x03,
            STRING = 0x04,
            REF = 0x05,
            REF_SLICE = 0x06,
            VECTOR = 0x10,  // no built-in types, schema of_map
            MAP = 0x11,     // no built-in types, schema of_map
            STRUCT = 0x12,  // no built-in types, schema of_struct
            EVENT = 0x13,   // no built-in types, schema of_map
            FN = 0x20,      // no built-in types, schema of_map
        }

        type TypeDef = {
            add: {
                kind: TypeKind
                children: VectorTypeDef
            },
            use: number
            is_const: boolean
        }

        enum ThreadState {
            IDLE = 0x00,
            RUNNING = 0x01,
            SLEEPING = 0x10,
            WAITING_DRIVER = 0x20,
            OK = 0xF0,
            ERROR = 0xFF
        }

        type PortDef = {
            name: string
            type_def: TypeDef
        }

        class Driver {
            public constructor(
                name: string,
                ports: VectorPortDef
            );
            public bind(__js_istance__: any): void;
            public render(data: {
                [port: number]: {
                    index: number,
                    type: string,
                    value: any
                }
            }): void;
        }

        class Thread {
            public constructor(
                node: Node,
                name: string,
                hex: string,
                ports: VectorPortDef
            );
            public get_state(): { value: ThreadState };
            public get_sleep(): number;
            public wakeup(): void;
            public reset(): void;
            public start(): void;
            public tick(): boolean;
        }

        class Node {
            public constructor(
                node: Engine,
                name: string,
                ports: VectorPortDef
            );
            public link_thread(thread: Thread): boolean
            public delete_thread(name: string): boolean
        }

        class Engine {
            public constructor(
                ports: VectorPortDef
            );
            public link_driver(driver: Driver): boolean
            public delete_driver(name: string): boolean
            public link_node(node: Node): boolean
            public delete_node(name: string): boolean
        }
        
    }

    const qb: Engine;
}

export type Kernel = {
    VectorTypeDef: typeof wasm.VectorTypeDef,
    VectorPortDef: typeof wasm.VectorPortDef,
    Driver: typeof wasm.Driver,
    Thread: typeof wasm.Thread,
    Node: typeof wasm.Node,
    Engine: typeof wasm.Engine,
}

declare function make_wasm_kernel (opts: {
    print: (...args: any[]) => void,
    printErr: (...args: any[]) => void,
}): Promise<Kernel>


export default make_wasm_kernel