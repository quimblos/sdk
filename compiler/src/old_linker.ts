import { Bytecode, CompilerNodes, QuimblosCompiler } from "./old_compiler";
import { quimblos } from "./lang/semantics";

export class QuimblosLinker {

    public static link(nodes: CompilerNodes, header: Bytecode, bytecode: Bytecode) {

        const intermediate: Bytecode = [...header];

        let port = 0;
        for (const n in nodes) {
            nodes[n]!.port = port;
            port++;
            intermediate.push(
                ...this.link_node(nodes[n]!)
            )
        }
        
        for (const byte of bytecode) {
            if (typeof byte !== 'object') continue;
            if (!('port' in byte)) continue;
            if (!byte.port.startsWith('__tmp')) continue;
            if (byte.port in nodes) continue;

            nodes[byte.port] = {
                statement: undefined as any,
                port
            }
            intermediate.push(
                { op: 'USE_NODE' },
                { type: 'ptr_short' },
                0x00,
                0x00
            )
            port++;
        }

        intermediate.push(...bytecode);

        this.preview(intermediate);

        const final = this.replace_symbols(nodes, intermediate)
        return this.to_hex(final)
    }

    private static link_node(node: CompilerNodes[string]): Bytecode {
        if (node.statement instanceof quimblos.VariableStatement) {
            let value: Bytecode;
            
            if (node.statement.value instanceof quimblos.Literal) {
                const literal = QuimblosCompiler.literal(node.statement.value);
                value = [{ type: literal.type }, ...literal.bytecode];
            }
            else {
                if (node.statement.identifier.type.arr_length !== undefined) {
                    value = QuimblosCompiler.arr(node.statement.identifier.type.name, node.statement.identifier.type.arr_length, true);
                    for (let i = 0; i < node.statement.identifier.type.arr_length; i++) {
                        switch (node.statement.identifier.type.name) {
                            case "err": value.push(...QuimblosCompiler.err('')); break;
                            case "bool": value.push(...QuimblosCompiler.bool(false)); break;
                            case "u8": value.push(...QuimblosCompiler.u8(0)); break;
                            case "i8": value.push(...QuimblosCompiler.i8(0)); break;
                            case "u16": value.push(...QuimblosCompiler.u16(0)); break;
                            case "i16": value.push(...QuimblosCompiler.i16(0)); break;
                            case "u32": value.push(...QuimblosCompiler.u32(0)); break;
                            case "i32": value.push(...QuimblosCompiler.i32(0)); break;
                            case "f32": value.push(...QuimblosCompiler.f32(0)); break;
                            case "str": value.push(...QuimblosCompiler.str('')); break;
                        }
                    }
                }
                else {
                    switch (node.statement.identifier.type.name) {
                        case "err": value = QuimblosCompiler.err('', true); break;
                        case "bool": value = QuimblosCompiler.bool(false, true); break;
                        case "u8": value = QuimblosCompiler.u8(0, true); break;
                        case "i8": value = QuimblosCompiler.i8(0, true); break;
                        case "u16": value = QuimblosCompiler.u16(0, true); break;
                        case "i16": value = QuimblosCompiler.i16(0, true); break;
                        case "u32": value = QuimblosCompiler.u32(0, true); break;
                        case "i32": value = QuimblosCompiler.i32(0, true); break;
                        case "f32": value = QuimblosCompiler.f32(0, true); break;
                        case "str": value = QuimblosCompiler.str('', true); break;
                    }
                }
            }

            const bytecode: Bytecode = [
                { op: 'USE_NODE' },
                ...value,

            ]
            return bytecode
        }
        return []
    }

    private static preview(intermediate: Bytecode) {
        const preview: Bytecode[] = [[]]
        for (const byte of intermediate) {
            if (typeof byte === 'object' && 'op' in byte) {
                preview.push([]);
            }
            if (typeof byte === 'object') {
                if ('op' in byte) preview.at(-1)!.push(byte.op);
                else if ('bind' in byte) preview.at(-1)!.push(byte.bind);
                else if ('type' in byte) preview.at(-1)!.push(byte.type);
                else if ('port' in byte) preview.at(-1)!.push(byte.port);
                else if ('device' in byte) preview.at(-1)!.push(byte.device);
            }
            else {
                preview.at(-1)!.push(byte);
            }
        }
        for (const line of preview) {
            console.log(line.join(' '));
        }
    }

    private static to_hex(final: (number|string)[]) {
        return final.map(v =>
            typeof v === 'string'
            ? v.charCodeAt(0).toString(16).padStart(2,'0')
            : v.toString(16).padStart(2,'0')
        ).join('');
    }

    private static replace_symbols(nodes: CompilerNodes, intermediate: Bytecode) {
        const bytecode: (number|string)[] = []
        for (const byte of intermediate) {
            if (typeof byte !== 'object') {
                bytecode.push(byte);
                continue;
            }

            if ('op' in byte) {
                bytecode.push(QuimblosCompiler.OP_CODE[byte.op])
            }
            else if ('bind' in byte) {
                bytecode.push(QuimblosCompiler.OP_BIND[byte.bind])
            }
            else if ('type' in byte) {
                bytecode.push(QuimblosCompiler.TYPE[byte.type])
            }
            else if ('port' in byte) {
                if (byte.device === '__script__') {
                    // TODO: get nearest node
                    const node = byte.port.startsWith('__tmp')
                        ? nodes[byte.port]!
                        : nodes[byte.port+byte.block]!;
                    bytecode.push(node.port!)
                }
                else
                    // TODO
                    bytecode.push(0x00)
            }
            else if ('device' in byte) {
                if (byte.device === '__script__') bytecode.push(0xFF);
                // TODO
                else bytecode.push(0x00);
            }
        }
        return bytecode
    }

}