import { qbscript } from "../grammar/semantics";
import { Bytecode, CompilerNodes, QuimblosCompiler } from "./compiler";

export class QuimblosLinker {

    public static link(nodes: CompilerNodes, header: Bytecode, bytecode: Bytecode) {

        let final = [...header];        

        let port = 0;
        for (const n in nodes) {
            nodes[n].port = port;
            port++;
            final.push(
                ...this.link_node(nodes[n])
            )
        }

        for (const byte of bytecode) {
            if (typeof byte === 'object') {
                const node = nodes[byte.$];
                final.push(node.port!);
            }
            else {
                final.push(byte);
            }
        }

        const hex = final.map(v =>
            typeof v === 'string'
            ? v.charCodeAt(0).toString(16).padStart(2,'0')
            : v.toString(16).padStart(2,'0')
        ).join('');

        return hex;
    }

    private static link_node(node: CompilerNodes[string]) {
        if (node.statement instanceof qbscript.VariableStatement) {
            let value: Bytecode;
            
            if (node.statement.value instanceof qbscript.Literal) {
                value = QuimblosCompiler.literal(node.statement.value);
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

            const bytecode = [
                QuimblosCompiler.OP_CODE.USE_NODE,
                ...value,

            ]
            console.log(bytecode);
            return bytecode
        }
        return []
    }

}