import { Instruction, NodeSpec, Ref, Type, Value } from "./compiler";
import { qasm } from "./bind";
import { quimblos } from "./lang/semantics";

export type Bytecode = number[]

export class QuimblosLinker {

    private constructor(
        private nodes: Record<string, NodeSpec>,
        private code: Instruction[]
    ) {
        console.log(nodes);
    }

    public static link(nodes: Record<string, NodeSpec>, code: Instruction[]) {
        const linker = new QuimblosLinker(nodes, code);
        return linker.link();
    }

    public static to_hex(code: Bytecode) {
        return code.map(v => v.toString(16).padStart(2,'0')).join('');
    }

    public link(): Bytecode {
        const bytecode: Bytecode = ['q'.charCodeAt(0),'b'.charCodeAt(0),0,0];

        for (const instr of this.code) {
            console.log(instr);
            switch (instr.op) {
                case "USE_DEVICE":
                    bytecode.push(qasm.OpCode.USE_DEVICE, ...this.str(instr.name));
                    break;
                case "USE_NODE":
                    bytecode.push(qasm.OpCode.USE_NODE, ...this.value(instr.type, true));
                    break;
                case "SET":

                    break;
                case "SLEEP":
                    bytecode.push(qasm.OpCode.SLEEP, ...this.u32(instr.time));
                    break;
                case "LOG":
                    bytecode.push(qasm.OpCode.LOG, this.device(instr.device), ...this.value(instr.type, instr.value, true));
                    break;
            }
        }
        return bytecode
    }


    private type(type: Type): Bytecode {
        if (type.name === 'arr') {
            return [qasm.Type.arr, ...this.u16(type.length)]
        }
        else return [qasm.Type[type.name]]
    }

    private device(name: string): number {
        if (name === '_$_') return 0xFF;
        throw new Error(`Non-script device not implemented yet`);
    }

    private value(type: Type, value: Value|Ref, include_type?: boolean): Bytecode {
        if (type.name === 'arr') {
            throw new Error(`Value of array not implemented yet`)
        }
        else switch (type.name) {
            case "err": return this.err(value as string, include_type);
            case "ptr": throw new Error('Value of pointer not implemented yet')
            case "ptr_short": throw new Error('Value of pointer not implemented yet')
            case "bool": return this.bool(value as boolean, include_type);
            case "u8": return this.u8(value as number, include_type);
            case "i8": return this.i8(value as number, include_type);
            case "u16": return this.u16(value as number, include_type);
            case "i16": return this.i16(value as number, include_type);
            case "u32": return this.u32(value as number, include_type);
            case "i32": return this.i32(value as number, include_type);
            case "f32": return this.f32(value as number, include_type);
            case "str": return this.str(value as string, include_type);
            case "str_short": return this.str(value as string, include_type);
        }
        return []
    }


    private bool (val: boolean, type?: boolean): Bytecode {
        return [
            ...(type ? [qasm.Type.bool] : []),
            val ? 0x01 : 0x00
        ];
    }

    private u8 (val: number, type?: boolean): Bytecode {
        if (val < 0) throw new Error(`Negative number ${val} cannot be compiled to u8`);
        if (val > 2**8) throw new Error(`Number ${val} is too large, cannot be compiled to u8`);
        if (type) return [qasm.Type.u8, val];
        return [val];
    }

    private i8 (val: number, type?: boolean): Bytecode {
        if (type) return [qasm.Type.i8, val];
        return [val];
    }

    private u16 (val: number, type?: boolean): Bytecode {
        if (val < 0) throw new Error(`Negative number ${val} cannot be compiled to u16`);
        if (val > 2**16) throw new Error(`Number ${val} is too large, cannot be compiled to u16`);
        const arr = new ArrayBuffer(2);
        const view = new DataView(arr);
        view.setUint16(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [qasm.Type.u16, ...blob];
        return [...blob];
    }

    private i16 (val: number, type?: boolean): Bytecode {
        const arr = new ArrayBuffer(2);
        const view = new DataView(arr);
        view.setUint16(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [qasm.Type.i16, ...blob];
        return [...blob];
    }

    private u32 (val: number, type?: boolean): Bytecode {
        if (val < 0) throw new Error(`Negative number ${val} cannot be compiled to u32`);
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setUint32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [qasm.Type.u32, ...blob];
        return [...blob];
    }

    private i32 (val: number, type?: boolean): Bytecode {
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setInt32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [qasm.Type.i32, ...blob];
        return [...blob];
    }

    private f32 (val: number, type?: boolean): Bytecode {
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setFloat32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [qasm.Type.f32, ...blob];
        return [...blob];
    }

    private err (val: string, type?: boolean): Bytecode {
        const valbytes = val.split('').map(v => v.charCodeAt(0));
        return [
            ...(type ? [qasm.Type.err] : []),
            ...valbytes
        ];
    }

    private str (val: string, type?: boolean): Bytecode {
        const valbytes = val.split('').map(v => v.charCodeAt(0));
        return [
            ...(type ? [qasm.Type.str] : []),
            ...valbytes
        ];
    }

    private arr (item_type: quimblos.Type, length: number, type?: boolean): Bytecode {
        return [
            ...(type ? [qasm.Type.arr] : []),
            qasm.Type[item_type],
            ...this.u16(length)
        ];
    }


}