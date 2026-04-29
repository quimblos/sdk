import { Instruction, Node, NodeSpec, Ref, Type, Value } from "./compiler";
import { qasm } from "./bind";
import { quimblos } from "./lang/semantics";
import { Kernel } from "./kernel";

export type Bytecode = number[]

export class QuimblosLinker {

    private constructor(
        private kernel: Kernel,
        private macros: Instruction[],
        private nodes: Record<string, NodeSpec>,
        private code: Instruction[]
    ) {
        for (const macro of macros) {
            console.log(macro);
        }
        for (const node of Object.entries(nodes)) {
            console.log(node);
        }
    }

    public static link(kernel: Kernel, macros: Instruction[], nodes: Record<string, NodeSpec>, code: Instruction[]) {
        const linker = new QuimblosLinker(kernel, macros, nodes, code);
        return linker.link();
    }

    public static to_hex(code: Bytecode) {
        return code.map(v => v.toString(16).padStart(2,'0')).join('');
    }

    public link(): Bytecode {
        const bytecode: Bytecode = ['q'.charCodeAt(0),'b'.charCodeAt(0),0,0];

        for (const macro of this.macros) {
            switch (macro.op) {
                case 'USE_DEVICE':
                    bytecode.push(qasm.OpCode.USE_DEVICE, ...this.str(macro.name));
                    break;
            }
            break;
        }

        for (const name in this.nodes) {
            const node = this.nodes[name]!;
            bytecode.push(qasm.OpCode.USE_VAR, ...this.data(node?.type, undefined, true));
            break;
        }

        const addresses: Record<string, number> = {};
        for (let i = 0; i < this.code.length; i++) {
            const instr = this.code[i]!;
            if ('$addr' in instr) {
                addresses[instr.$addr!] = i;
            }
        }
        for (let i = 0; i < this.code.length; i++) {
            const instr = this.code[i]!;
            instr.$addr = i as any;
            switch (instr.op) {
                case "BRANCH":
                    instr.true_addr = addresses[instr.true_addr[0]]!+instr.true_addr[1] as any;
                    instr.false_addr = addresses[instr.false_addr[0]]!+instr.false_addr[1] as any;
                    break;
                case "GOTO":
                    instr.code_addr = addresses[instr.code_addr[0]]!+instr.code_addr[1] as any;
                    break;
            }
            console.log(instr);
        }

        for (const instr of this.code) {
            switch (instr.op) {
                case "SET":
                    bytecode.push(qasm.OpCode.SET, ...this.target(instr.target), ...this.data(instr.source.type, instr.source.value, true));
                    break;
                case "SLEEP":
                    bytecode.push(qasm.OpCode.SLEEP, ...this.data(instr.time.type, instr.time.value, true));
                    break;
                case "GOTO":
                    bytecode.push(qasm.OpCode.GOTO, ...this.u16(instr.code_addr as any as number));
                    break;
                case "BRANCH":
                    bytecode.push(qasm.OpCode.BRANCH, ...this.data(instr.source.type, instr.source.value, true), ...this.u16(instr.true_addr as any as number), ...this.u16(instr.false_addr as any as number));
                    break;
                case "ADD":
                    bytecode.push(qasm.OpCode.ADD, ...this.target(instr.target), ...this.data(instr.source.type, instr.source.value, true));
                case "MULT":
                    bytecode.push(qasm.OpCode.MULT, ...this.target(instr.target), ...this.data(instr.source.type, instr.source.value, true));
                    break;
                case "LOG":
                    bytecode.push(qasm.OpCode.LOG, ...this.data(instr.source.type, instr.source.value, true));
                    break;
            }
        }
        console.log({bytecode});
        return bytecode
    }

    private target(node: Node): Bytecode {
        const ref = node.value as Ref;
        return [
            this.device(ref.device),
            0
        ]
    }

    private device(name: string): number {
        if (name === '_$_') return 0b01111111;
        const idx = this.macros.findIndex(macro =>
            macro.op === 'USE_DEVICE'
            && macro.name === name
        );
        if (idx < 0) throw new Error(`Linker error: Device '${name}' not found `);
        return idx;
    }

    private data(type: Type, value?: Value|Ref, include_type?: boolean): Bytecode {
        if (type.name === 'arr') {
            throw new Error(`Value of array not implemented yet`)
        }
        else switch (type.name) {
            case "err": return this.err(value as string ?? '', include_type);
            // case "ptr": throw new Error('Value of pointer not implemented yet')
            // case "ptr_short": throw new Error('Value of pointer not implemented yet')
            case "bool": return this.bool(value as boolean ?? false, include_type);
            case "u8": return this.u8(value as number ?? 0, include_type);
            case "i8": return this.i8(value as number ?? 0, include_type);
            case "u16": return this.u16(value as number ?? 0, include_type);
            case "i16": return this.i16(value as number ?? 0, include_type);
            case "u32": return this.u32(value as number ?? 0, include_type);
            case "i32": return this.i32(value as number ?? 0, include_type);
            case "f32": return this.f32(value as number ?? 0, include_type);
            case "str": return this.str(value as string ?? '', include_type);
            // case "str_short": return this.str(value as string, include_type);
            case "null": throw new Error('Value of null not implemented yet')
            case "void": throw new Error('Value of void not implemented yet')
            case "ref": return this.ref(false, this.device((value as Ref).device), 0, 0, true);
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
            val.length,
            ...valbytes
        ];
    }

    private ref (deref: boolean, device: number, port: number, index: number, type?: boolean): Bytecode {
        return [
            ...(type ? [qasm.Type.ref] : []),
            (deref ? 0b10000000 : 0) | device,
            ...this.u8(port),
        ];
    }

    private arr (item_type: quimblos.Type, length: number, type?: boolean): Bytecode {
        return [
            ...(type ? [qasm.Type.array] : []),
            qasm.Type[item_type],
            ...this.u16(length)
        ];
    }


}