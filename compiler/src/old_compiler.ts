import { AST } from "@quimblos/langmaker";
import { quimblos } from "./lang/semantics";
import { QuimblosLinker } from "./linker";
import { Kernel } from "./kernel";

export type Bytecode = (
    number
    | string
    | {op: keyof typeof QuimblosCompiler.OP_CODE}
    | {bind: keyof typeof QuimblosCompiler.OP_BIND}
    | {type: keyof typeof QuimblosCompiler.TYPE}
    | {device:string}
    | {port:string, device: string, block:number}
)[]

export type CompilerNodes = Record<string, {
    statement: quimblos.VariableStatement|quimblos.PointerStatement
    port?: number
}>;

type CompilerBlock = {
    idx: number
    nodes: CompilerNodes
    bytecode: Bytecode
}
type CompilerChunk = {
    preset: Bytecode
    bytecode: Bytecode
    type: keyof typeof QuimblosCompiler.TYPE
    depth: number
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
        str: 0xF0,
        str_short: 0xF0,
        arr: 0xE0,
        arr_short: 0xE1,
    };

    public static compile(kernel: Kernel, ast: AST) {
        if (ast.errors.length) {
            console.error(`Script contains errors. Compilation skipped.`)
            return;
        }
        const script = ast.root as quimblos.Script;

        const nodes: CompilerNodes = {};
        let header: Bytecode = [
            ...this.HEADER
        ];
        let bytecode: Bytecode = [];

        for (const macro of script.macros) {
            if (macro instanceof quimblos.UseDeviceMacro) {
                header.push(
                    ...this.compile_use_device_macro(macro)
                )
            }
        }

        let idx = 0;
        for (const block of script.blocks) {
            const compiled = this.compile_block(block, idx);
            
            Object.assign(nodes, compiled.nodes);
            bytecode.push(...compiled.bytecode);

            idx = compiled.idx+1;
        }

        return QuimblosLinker.link(nodes, header, bytecode);
    }

    private static compile_use_device_macro(node: quimblos.UseDeviceMacro): Bytecode {
        return [
            {op: 'USE_DEVICE' as const},
            node.device.length,
            ...node.device.split('')
        ]
    }

    // Blocks

    private static compile_block(node: quimblos.Block, block: number) {
        const compiled: CompilerBlock = {
            idx: block,
            nodes: {},
            bytecode: []
        }

        for (const statement of node.statements) {
            if (statement instanceof quimblos.VariableStatement) {
                compiled.nodes[statement.identifier.name+block] = { statement };
                compiled.bytecode.push(
                    ...this.compile_variable_statement(statement, block)
                );
            }
            else if (statement instanceof quimblos.PointerStatement) {
                compiled.nodes[statement.identifier.name+block] = { statement };
                compiled.bytecode.push(
                    ...this.compile_pointer_statement(statement, block)
                );
            }
            else if (statement instanceof quimblos.AssignStatement) {
                compiled.bytecode.push(
                    ...this.compile_assign_statement(statement, block)
                );
            }
            else if (statement instanceof quimblos.SleepStatement) {
                compiled.bytecode.push(
                    ...this.compile_sleep_statement(statement)
                );
            }
            else if (statement instanceof quimblos.LogStatement) {
                compiled.bytecode.push(
                    ...this.compile_log_statement(statement, block)
                );
            }
        }

        return compiled;
    }

    // Statements

    private static compile_variable_statement(node: quimblos.VariableStatement, block: number) {
        let bytecode: Bytecode = []
        if (node.value) {
            const ref = new quimblos.Reference();
            ref.node = node.identifier.name;
            bytecode.push(
                ...this.set(ref, node.value, block)
            )
        }
        return bytecode;
    }

    private static compile_pointer_statement(node: quimblos.PointerStatement, block: number): Bytecode {
        let bytecode: Bytecode = []

        if (node.value) {
            bytecode.push(
                ...this.set(node.ref, node.value, block)
            )
        }
        return bytecode;
    }

    private static compile_assign_statement(node: quimblos.AssignStatement, block: number): Bytecode {
        const bytecode = this.set(node.target, node.source, block);
        return bytecode
    }

    private static compile_sleep_statement(node: quimblos.SleepStatement): Bytecode {
        const bytecode: Bytecode = [
            { op: 'SLEEP' },
            ...this.u32(node.time)
        ]
        return bytecode
    }

    private static compile_log_statement(node: quimblos.LogStatement, block: number): Bytecode {
        const chunk = this.prepare_expr(node.value, block, 0);
        const bytecode: Bytecode = [
            ...chunk.preset,
            { op: 'LOG' },
            0xFF, // TODO
            { type: chunk.type },
            ...chunk.bytecode
        ]
        return bytecode
    }

    // Set Meta-Expression

    private static set(target: quimblos.Reference, source: quimblos.Expression, block: number): Bytecode {
        
        const bytecode: Bytecode = [];

        const target_chunk = this.prepare_ptr(target, block, 0);
        bytecode.push(...target_chunk.preset);

        const source_chunk = this.prepare_expr(source, block, target_chunk.depth);
        bytecode.push(...source_chunk.preset);

        let bind: keyof typeof this.OP_BIND;
        if (source_chunk.type === 'ptr') {
            if (target_chunk.type === 'ptr' && target_chunk.depth > 0) bind = 'REF_REF';
            else bind = 'NODE_REF';
        }
        else {
            if (target_chunk.type === 'ptr' && target_chunk.depth > 0) bind = 'REF_NODE';
            else bind = 'NODE_NODE';
        }

        bytecode.push(
            { op: 'SET' },
            { bind },
            ...target_chunk.bytecode,
            { type: source_chunk.type },
            ...source_chunk.bytecode,
        );

        return bytecode;
    }

    // Value/Type

    public static value(node: quimblos.Value, block: number, depth: number): CompilerChunk {
        if (node instanceof quimblos.Reference) {
            return this.prepare_ptr(node, block, depth);
        }
        else if (node instanceof quimblos.Literal) {
            const literal = this.literal(node);
            return {
                preset: [] as Bytecode,
                bytecode: literal.bytecode,
                type: literal.type,
                depth
            }
        }
        else { // if (node instanceof quimblos.Expression)
            return this.prepare_expr(node, block, depth)
        }
    }

    public static literal(node: quimblos.Literal) {
        switch (node.literal_type) {
            case "Boolean":
                return { type: 'bool' as const, bytecode: this.bool(node.value) }
            case "Hexcode":
                // TODO
                return { type: 'u32' as const, bytecode: [0,0,0,0] }
            case "Bitmask":
                return { type: 'u32' as const, bytecode: [0,0,0,0] }
            case "Float":
                return { type: 'f32' as const, bytecode: this.f32(node.value) }
            case "UnsignedInteger":
                return { type: 'u32' as const, bytecode: this.u32(node.value) }
            case "Integer":
                return { type: 'i32' as const, bytecode: this.i32(node.value) }
            case "String":
                return { type: 'str' as const, bytecode: this.str(node.value) }
        }
    }

    public static type(node: quimblos.TypeIdentifier, string: string = '', error = 0) {
        // Array
        if (node.arr_length) {
            return [
                {type: 'arr' as const},
                this.TYPE[node.name],
                ...this.u16(node.arr_length)
            ]
        }
        // String
        if (node.name === 'str') {
            return [
                {type: 'str' as const},
                ...this.u16(string.length)
            ]
        }
        // Error
        if (node.name === 'err') {
            return [
                {type: 'err' as const},
                error,
                string.length
            ]
        }
        // Single
        return [
            this.TYPE[node.name]
        ];
    }

    // Expressions
    
    private static prepare_expr (node: quimblos.Expression, block: number, depth: number): CompilerChunk {
        // TODO
        return this.expression(node, block, depth);
    }

    private static expression(node: quimblos.Expression, block: number, depth: number): CompilerChunk {

        if (node.terms.length === 1) {
            return this.value(node.terms[0]!.value, block, depth);
        }

        const preset: Bytecode = []
        for (let i = 0; i < node.terms.length; i++) {
            const term = node.terms[i]!;
            const value = this.value(term.value, block, depth);
            preset.push(...value.bytecode);

            if (i < node.terms.length-1) {
                const op = node.ops[i]!;
                if (['and','or'].includes(op)) {
                }
                else if (['==','!=','>','<','>=','<='].includes(op)) {
                    preset.push(

                    )
                }
                else if (['+','-','*','/','%','^'].includes(op)) {
                    preset.push(

                    )
                }
            }
        }

        // TODO
        return {
            preset,
            bytecode: [],
            depth,
            type: 'u8'
        }
    }


    // Pointers

    private static prepare_ptr (node: quimblos.Reference, block: number, depth: number): CompilerChunk {
        // Simple reference
        if (!node.index) {
            return {
                preset: [],
                bytecode: this.ptr(node, block, 0),
                type: 'ptr',
                depth
            }
        }
        
        // Indexed reference
        const index = this.prepare_expr(node.index, block, depth+1);
        
        return {
            preset: [
                ...index.preset,
                // Assign base pointer to out
                {op: 'SET' as const}, { bind: 'SHORT_NODE_NODE' as const}, {device: '__script__'}, {port: `__tmp_${depth}__`, device: '__script__', block}, ...this.ptr(node, block, 0, true),
                // Update out index
                ...(index.type === 'ptr' ?
                    // From value
                    [{op: 'SET' as const}, { bind: 'SHORT_NODE_REF' as const}, {device: '__script__'}, {port: `__tmp_${depth}__`, device: '__script__', block}, {type: index.type}, ...index.bytecode]
                    // From reference
                    : [{op: 'SET' as const}, { bind: 'SHORT_NODE_NODE' as const}, {device: '__script__'}, {port: `__tmp_${depth}__`, device: '__script__', block}, {type: index.type}, ...index.bytecode]
                )
            ],
            bytecode: [
                {device: '__script__'}, {port: `__tmp_${depth}__`, device: '__script__', block}, 0x00, 0x00
            ],
            type: 'ptr',
            depth: index.depth
        }
    }

    public static ptr (ref: quimblos.Reference, block: number, index?: number, type?: boolean): Bytecode {
        return [
            ...(type ? [{type: 'ptr' as const}] : []),
            ...this.port(ref.device, ref.node, block),
            ...this.u16(index ?? 0)
        ]
    }

    // Port

    private static port (device_name: string|undefined, node_name: string, block: number): Bytecode {
        // Device Port
        if (device_name) {
            return [{ device: device_name }, { port: node_name, device: device_name, block }];
        }
        // Script Port
        return [{device: '__script__'}, { port: node_name, device: '__script__', block }];
    }

    // Data

    public static bool (val: boolean, type?: boolean): Bytecode {
        return [
            ...(type ? [{type: 'bool' as const}] : []),
            val ? 0x01 : 0x00
        ];
    }

    public static u8 (val: number, type?: boolean): Bytecode {
        if (val < 0) throw new Error(`Negative number ${val} cannot be compiled to u8`);
        if (val > 2**8) throw new Error(`Number ${val} is too large, cannot be compiled to u8`);
        if (type) return [{type: 'u8' as const}, val];
        return [val];
    }

    public static i8 (val: number, type?: boolean): Bytecode {
        if (type) return [{type: 'i8' as const}, val];
        return [val];
    }

    public static u16 (val: number, type?: boolean): Bytecode {
        if (val < 0) throw new Error(`Negative number ${val} cannot be compiled to u16`);
        if (val > 2**16) throw new Error(`Number ${val} is too large, cannot be compiled to u16`);
        const arr = new ArrayBuffer(2);
        const view = new DataView(arr);
        view.setUint16(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [{type: 'u16' as const}, ...blob];
        return [...blob];
    }

    public static i16 (val: number, type?: boolean): Bytecode {
        const arr = new ArrayBuffer(2);
        const view = new DataView(arr);
        view.setUint16(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [{type: 'i16' as const}, ...blob];
        return [...blob];
    }

    public static u32 (val: number, type?: boolean): Bytecode {
        if (val < 0) throw new Error(`Negative number ${val} cannot be compiled to u32`);
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setUint32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [{type: 'u32' as const}, ...blob];
        return [...blob];
    }

    public static i32 (val: number, type?: boolean): Bytecode {
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setInt32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [{type: 'i32' as const}, ...blob];
        return [...blob];
    }

    public static f32 (val: number, type?: boolean): Bytecode {
        const arr = new ArrayBuffer(4);
        const view = new DataView(arr);
        view.setFloat32(0, val, false);
        const blob = new Uint8Array(arr);
        if (type) return [{type: 'f32' as const}, ...blob];
        return [...blob];
    }

    public static err (val: string, type?: boolean): Bytecode {
        if (type) return [{type: 'err' as const}, ...this.u16(val.length), ...val.split('')];
        return val.split('');
    }

    public static str (val: string, type?: boolean): Bytecode {
        return [
            ...(type ? [{type: 'str' as const}] : []),
            ...this.u16(val.length),
            ...val.split('')
        ];
    }

    public static arr (item_type: quimblos.Type, length: number, type?: boolean): Bytecode {
        return [
            ...(type ? [{type: 'arr' as const}] : []),
            {type: item_type}, ...this.u16(length)
        ];
    }

}