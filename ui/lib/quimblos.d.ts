import { quimblos } from "@quimblos/compiler/src/lang/semantics"
import { Engine } from "./kernel"

declare global {
    namespace wasm {
        class VectorCode {
            static from(items: number[]): VectorCode;
            public push_back(val: number): void;    
        }
        class VectorDeviceData {
            static from(items: DeviceData[]): VectorDeviceData;
            public push_back(val: DeviceData): void;
        }

        type res_Engine = {
            ok: boolean
            message: string
        }

        type DeviceData = {
            name: string
            bytes: VectorCode
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
            public link_device(device: Device): res_Engine;
            public get_device(name: string): Device;
            public delete_device(name: string): void;
            public make_runner(name: string, hex: string): res_Engine;
            public get_runner(name: string): Runner;
            public delete_runner(name: string): void;
        }

        class Runner {
            public start(): void
            public reset(): void
            public wakeup(): void
            public tick(): boolean
            public get_state(): { value: RunnerState };
            public get_cursor(): number;
            public get_sleep(): number;
            public get_name(): string;
        }

        class Device {
            static make(name: string, variables: VectorDeviceData): Device;
            public bind(device: any): void;
            public has_variable(port: number): boolean;
        }

    }

    const qb: Engine;
}

type Kernel = {
    VectorCode: typeof wasm.VectorCode,
    VectorDeviceData: typeof wasm.VectorDeviceData,
    Engine: typeof wasm.Engine,
    Device: typeof wasm.Device,
}

declare function WASM (opts: {
    print: (...args: any[]) => void,
    printErr: (...args: any[]) => void,
}): Promise<Kernel>

export { Kernel }
export default WASM