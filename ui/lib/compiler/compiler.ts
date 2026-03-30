import { qbscript } from "../grammar/semantics";
import { AST } from "../lang-maker/ast";

type CompilerAST = {
    bytecode: (string|number)[]
}

export class QuimblosCompiler {

    public static HEADER = ['q','b',0,0];

    public static SCRIPT_PTR_NODE0 = [0xFF, 0x00, 0x00, 0x00]
    public static SCRIPT_PTR_NODE1 = [0xFF, 0x01, 0x00, 0x00]
    public static SCRIPT_PTR_NODE2 = [0xFF, 0x02, 0x00, 0x00]

    public static OP_BIND = {
        NODE_NODE: 0x00,
        NODE_REF: 0x01,
        REF_NODE: 0x02,
        REF_REF: 0x03,
        SHORT_NODE_NODE: 0x10,
        SHORT_NODE_REF: 0x11,
        SHORT_REF_NODE: 0x12,
        SHORT_REF_REF: 0x13
    };

    public static OP_CODE = {
        USE_DEVICE: 0x01,
        USE_NODE: 0x02,
        USE_NODE_ALIASED: 0x03,
        SET: 0x10,
        HOLD: 0x1A,
        RELEASE: 0x1B,
        GOTO: 0x20,
        IF_EQ: 0x21,
        IF_LT: 0x25,
        IF_GT: 0x23,
        ADD: 0x30,
        SUB: 0x31,
        MULT: 0x32,
        DIV: 0x33,
        MOD: 0x34,
        POW: 0x35,
        LOG: 0xD0,
        SLEEP: 0xE0,
        RETURN: 0xEE,
        RESET: 0xF0,
        REBOOT: 0xFF,
    };

    public static TYPE = {
        void: 0x00,
        err: 0xFF,
        ptr: 0x02,
        ptr_short: 0x01,
        bool: 0x0A,
        u8: 0x10,
        i8: 0x11,
        u16: 0x20,
        i16: 0x21,
        u32: 0x40,
        i32: 0x41,
        f32: 0x4A,
        str: 0xE0,
        str_short: 0xE0,
        arr: 0xF0,
        arr_short: 0xF1,
    };

    public static compile(ast: AST) {
        console.clear();
        if (ast.errors.length) {
            console.error(`Script contains errors. Compilation skipped.`)
            return;
        }

        ast.traverse(node => {
            if (node instanceof qbscript.UseDeviceMacro) {
                this.compile_use_device_macro(node);
                return true;
            }
            if (node instanceof qbscript.VariableDeclaration) {
                this.compile_variable_declaration(ast, node);
                return true;
            }
            if (node instanceof qbscript.PointerDeclaration) {
                this.compile_pointer_declaration(ast, node);
                return true;
            }
            if (node instanceof qbscript.AssignStatement) {
                this.compile_assign_statement(ast, node);
                return true;
            }
            else if (node instanceof qbscript.SleepStatement) {
                this.compile_sleep_statement(node);
                return true;
            }
            return
        })

        let bytecode: (number | string)[] = [
            ...this.HEADER
        ];
        ast.traverse(node => {
            const cnode = node as any as CompilerAST;
            if (!cnode.bytecode?.length) return;
            bytecode.push(...cnode.bytecode);
        }, 'up')

        const hex = bytecode.map(v =>
            typeof v === 'string'
            ? v.charCodeAt(0).toString(16).padStart(2,'0')
            : v.toString(16).padStart(2,'0')
        ).join('');

        return { bytecode, hex }
    }

    private static compile_use_device_macro(node: qbscript.UseDeviceMacro) {
        (node as any as CompilerAST).bytecode = [
            this.OP_CODE.USE_DEVICE,
            node.device.length,
            ...node.device.split('')
        ]
    }

    private static compile_variable_declaration(ast: AST, node: qbscript.VariableDeclaration) {
        (node as any as CompilerAST).bytecode = [
            this.OP_CODE.USE_NODE,
            ...this.type(node.identifier.type)
        ]
        if (node.value) {
            // TODO: variable initialization
            // (node as any as CompilerAST).bytecode.push(
            //     ...this.set(ast, node.ref, node.value)
            // )
        }
    }

    private static compile_pointer_declaration(ast: AST, node: qbscript.PointerDeclaration) {
        (node as any as CompilerAST).bytecode = [
            this.TYPE.ptr,
            ...this.ptr(ast, node.ref)
        ]

        if (node.value) {
            (node as any as CompilerAST).bytecode.push(
                ...this.set(ast, node.ref, node.value)
            )
        }
    }

    private static compile_assign_statement(ast: AST, node: qbscript.AssignStatement) {
        (node as any as CompilerAST).bytecode = this.set(ast, node.target, node.source);
    }

    private static compile_sleep_statement(node: qbscript.SleepStatement) {
        (node as any as CompilerAST).bytecode = [
            this.OP_CODE.SLEEP,
            ...this.u32(node.time)
        ]
    }

    // Set Meta-Expression

    private static set(ast: AST, target: qbscript.Reference, source: qbscript.MathExpression | qbscript.Value | qbscript.Reference) {
        
        const bytecode = [];

        const target_ptr = this.prepare_ptr(ast, target);
        bytecode.push(...target_ptr.preset);

        let source_ptr;
        if (source instanceof qbscript.Reference) {
            source_ptr = this.prepare_ptr(ast, source, target_ptr.tmp_node);
        }
        else if (source instanceof qbscript.Value) {
            source_ptr = {
                preset: [],
                bytecode: this.value(source),
                tmp_node: undefined
            }
        }
        else if (source instanceof qbscript.MathExpression) {
            source_ptr = {
                preset: [],
                bytecode: this.math_expression(source),
                tmp_node: undefined
            }
        }
        bytecode.push(...source_ptr.preset);

        let bind;
        if (target_ptr.tmp_node === undefined) {
            if (source_ptr.tmp_node === undefined) bind = this.OP_BIND.NODE_NODE
            else bind = this.OP_BIND.NODE_REF
        }
        else {
            if (source_ptr.tmp_node === undefined) bind = this.OP_BIND.REF_NODE
            else bind = this.OP_BIND.REF_REF
        }

        // console.log({target_ptr, source_ptr, bind})

        bytecode.push(
            this.OP_CODE.SET,
            bind,
            ...target_ptr.bytecode,
            ...source_ptr.bytecode,
        );

        return bytecode;
    }

    // Value/Type

    private static math_expression(node: qbscript.MathExpression) {
        // TODO
        return this.value(node.terms[0].value as qbscript.Value);
    }

    private static value(node: qbscript.Value) {
        switch (node.value_type) {
            case "Boolean":
                return this.bool(node.value, true)
            case "Hexcode":
                // TODO
                return []
            case "Bitmask":
                // TODO
                return []
            case "Float":
                return this.f32(node.value, true)
            case "UnsignedInteger":
                return this.u32(node.value, true)
            case "Integer":
                return this.i32(node.value, true)
            case "String":
                return this.str(node.value, true)
        }
    }

    private static type(node: qbscript.TypeIdentifier, string: string = '', error = 0) {
        // Array
        if (node.arr_length) {
            return [
                this.TYPE.arr,
                this.TYPE[node.name],
                ...this.u16(node.arr_length)
            ]
        }
        // String
        if (node.name === 'str') {
            return [
                this.TYPE.str,
                ...this.u16(string.length)
            ]
        }
        // Error
        if (node.name === 'err') {
            return [
                this.TYPE.err,
                error,
                string.length
            ]
        }
        // Single
        return [
            this.TYPE[node.name]
        ];
    }

    // Pointers

    private static prepare_ptr (ast: AST, ref: qbscript.Reference, block_tmp_node?: 0|1|2) {

        const chain: qbscript.Reference[] = [ref];
        let index: number|undefined = undefined;
        const parse_chain = (ref: qbscript.Reference) => {
            if (ref.index instanceof qbscript.Reference) {
                chain.push(ref.index);
                parse_chain(ref.index);
            }
            else index = ref.index;
        }
        parse_chain(ref);

        if (chain.length === 1) {
            return {
                preset: [] as number[],
                bytecode: this.ptr(ast, chain[0]),
                tmp_node: undefined
            }
        }
        
        let tmp_nodes = [0,1,2]
            .filter(v => v !== block_tmp_node)
            .map(v => this[`SCRIPT_PTR_NODE${v as 0|1|2}`]);

        let preset = [];
        // ...[a[b[c[d]]]]
        if (index) {
            preset.push(
                // Assign c to 0x00
                this.OP_CODE.SET, this.OP_BIND.NODE_NODE, ...tmp_nodes[0], this.TYPE.ptr, ...this.ptr(ast, chain.at(-1)),
                // Assign d to index of 0x00
                this.OP_CODE.SET, this.OP_BIND.NODE_REF, ...tmp_nodes[0], ...this.u16(index, true),
            )
        }
        else {
            preset.push(
                // Assign c to 0x00
                this.OP_CODE.SET, this.OP_BIND.NODE_NODE, ...tmp_nodes[0], this.TYPE.ptr, ...this.ptr(ast, chain.at(-2)),
                // Assign d to index of 0x00
                this.OP_CODE.SET, this.OP_BIND.NODE_REF, ...tmp_nodes[0], this.TYPE.ptr, ...this.ptr(ast, chain.at(-1)),
            )
        }
        let t = 1;
        for (let j = chain.length-(index?2:3); j >= 0; j--) {
            let last_t = t ? 0 : 1;
            preset.push(
                // Assign b to tmp
                this.OP_CODE.SET, this.OP_BIND.NODE_NODE, ...tmp_nodes[t], this.TYPE.ptr, ...this.ptr(ast, chain[j]),
                // Assign last tmp to index of 0x00
                this.OP_CODE.SET, this.OP_BIND.NODE_REF, ...tmp_nodes[t], this.TYPE.ptr, ...tmp_nodes[last_t],
            )
            t = last_t;
        }

        return {
            preset,
            bytecode: tmp_nodes[t],
            tmp_node: tmp_nodes[t][1] as 0|1|2
        }
    }

    private static ptr (ast: AST, ref: qbscript.Reference) {
        return [
            this.device(ref.device),
            this.port(ast, ref.device, ref.node),
            ...this.u16(ref.index instanceof qbscript.Reference ? 0 : ref.index)
        ]
    }

    // Device/Port

    private static device (name?: string) {
        if (!name) return 0xFF; // script

        const idx = qb.get_device_index(name);
        if (idx < 0) throw new Error(`Device ${name} not found`);
        return idx;
    }

    private static port (ast: AST, device_name: string|undefined, node_name: string) {
        if (device_name) {
            const device = qb.get_device(device_name);
            if (!device) throw new Error(`Device ${device_name} not found`);
            const idx = device.nodes.findIndex(node => node.name === node_name);
            if (idx < 0) throw new Error(`Device ${device_name} doesn't have a node named ${node_name}`);
            return idx;
        }
        const idx = (ast.root as qbscript.Script).declarations.findIndex(d => 
            d instanceof qbscript.VariableDeclaration
            && d.identifier.name === node_name
        )
        if (idx < 0) throw new Error(`Script doesn't have a node named ${node_name}`);
            return idx;
    }

    // Data

    private static bool (val: boolean, type?: boolean) {
        if (type) return [this.TYPE.bool, val ? 0x01 : 0x00];
        return [val ? 0x01 : 0x00];
    }

    private static u16 (val: number, type?: boolean) {
        const arr = new ArrayBuffer(2);
        const view = new DataView(arr);
        view.setUint16(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [this.TYPE.u16, ...blob];
        return [...blob];
    }

    private static u32 (val: number, type?: boolean) {
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setUint32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [this.TYPE.u32, ...blob];
        return [...blob];
    }

    private static i32 (val: number, type?: boolean) {
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setInt32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [this.TYPE.i32, ...blob];
        return [...blob];
    }

    private static f32 (val: number, type?: boolean) {
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setFloat32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [this.TYPE.f32, ...blob];
        return [...blob];
    }

    private static str (val: string, type?: boolean) {
        if (type) return [this.TYPE.str, ...this.u16(val.length), ...val.split('')];
        return val.split('');
    }

}

// public get hex() {
//   if (this.script.errors.length) return;

  

//   for (const node of this.script.ast) {
//     switch (node.$) {
//       case 'use_device': {
//         const name = node.device.name;
//         vec.push(op_code.USE_DEVICE, name.length, ...name.split(''))
//       } break;
//       case 'set': {
//         vec.push(op_code.SET, op_bind.SHORT_NODE_NODE, node.target.device.index, node.target.node.index, type[node.target.type], node.value)
//       } break;
//       case 'sleep': {
//         vec.push(op_code.SLEEP, 0, 0, 5, 50);
//       } break;
//     }
//   }

//   const hex = vec.map(v =>
//     typeof v === 'string'
//     ? v.charCodeAt(0).toString(16).padStart(2,'0')
//     : v.toString(16).padStart(2,'0')
//   ).join('');
//   console.log({vec,hex});
//   return hex;
//   // return '7162000001064c4544424152101000001055e0000003e81010000010aae0000003e8200000';
// }