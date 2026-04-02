import { quimblos } from "@quimblos/compiler/src/lang/semantics"
import { Engine } from "./kernel"

declare global {
    
    namespace wasm {
        class VectorString {
            public push_back(val: string): void;    
        }
        class VectorDeviceNode {
            public push_back(val: DeviceNode): void;    
        }

        type res_Engine = {
            ok: boolean
            message: string
            runner?: any
        }

        type DeviceNode = {
            name: string
            type: quimblos.Type
            arr_length?: number
        }

        enum RunnerState {
            IDLE = 0x00,
            RUNNING = 0x01,
            SLEEPING = 0x10,
            OK = 0xF0,
            ERROR = 0xFF
        }

        class Engine {
            public constructor();
            public put_device(device: Device): res_Engine;
            public make_runner(name: string, hex: string): number;
            public get_runner(name: string): Runner;
            public delete_runner(name: string): res_Engine;
        }

        class Runner {
            public getState(): { value: number };
            public getSleep(): number;
            public start(): void
            public tick(): boolean
            public reset(): void
            public wakeup(): void
        }

        class Device {
            public constructor(name: string, regs: VectorDeviceNode);
            public has_i(port: number): boolean;
            public bind(device: any): void;
        }

    }

    const qb: Engine;
}

type Kernel = {
    VectorString: typeof wasm.VectorString,
    Engine: typeof wasm.Engine,
    Device: typeof wasm.Device,
}

declare function WASM (opts: {
    print: (...args: any[]) => void,
    printErr: (...args: any[]) => void,
}): Promise<Kernel>

export { Kernel }
export default WASM