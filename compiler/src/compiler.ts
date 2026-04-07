import { AST } from "@quimblos/langmaker";
import { qasm } from "./bind";
import { quimblos } from "./lang/semantics";
import { Kernel } from "./kernel";
import { QuimblosLinker } from "./linker";

export type Ref = {
    device?: string|undefined
    node: string
    index?: number
}
export type Value = boolean|number|string|boolean[]|number[]|string[]

export type Type =
    { name: Exclude<keyof qasm.Type, 'arr'|'ptr'> }
    | { name: 'ptr' }
    | { name: 'arr', item: keyof qasm.Type, length: number }

export type Instruction = 
    { op: 'USE_DEVICE', name: string }
    | { op: 'USE_NODE', name: string, type: Type }
    | { op: 'SET', bind: keyof qasm.OpBind, target: Ref, source: Source }
    | { op: 'ADD', target: Ref, source: Source }
    | { op: 'SUB', target: Ref, source: Source }
    | { op: 'MULT', target: Ref, source: Source }
    | { op: 'DIV', target: Ref, source: Source }
    | { op: 'MOD', target: Ref, source: Source }
    | { op: 'POW', target: Ref, source: Source }
    | { op: 'SLEEP', time: number }
    | { op: 'LOG', device: string, source: Source }


type TypeSpec = {
    name: keyof qasm.Type | 'ptr'
    arr_length?: number | undefined
}
export type NodeSpec = {
    type: TypeSpec
    name: string
}
type Source = {
    type: Type
    value: Value|Ref
}
type Chunk = {
    prepare: Instruction[]
    out: Source
}

export class QuimblosCompiler {

    private root: quimblos.Script;
    private nodes: Record<string, NodeSpec> = {};

    private code: Instruction[] = [];

    private constructor(
        public ast: AST
    ) {
        this.root = ast.root as quimblos.Script;
    }

    public static compile(kernel: Kernel, ast: AST) {
        const compiler = new QuimblosCompiler(ast);
        const code = compiler.compile();
        return { nodes: compiler.nodes, code };
    }

    public compile() {
        this.code = [];
        for (const node of this.root.macros) {
            if (node instanceof quimblos.UseDeviceMacro) {
                this.code.push(this._use_device(node.device))
            }
        }
        for (const block of this.root.blocks) {
            this.code.push(...this.compile_block(block));
        }
        return this.code;
    }

    private compile_block(node: quimblos.Block): Instruction[] {
        const code: Instruction[] = [];

        for (const statement of node.statements) {
            if (statement instanceof quimblos.VariableStatement) {
                this.make_symbol(statement.identifier.name, statement.identifier.type)
            }
            else if (statement instanceof quimblos.PointerStatement) {
                this.make_symbol(statement.identifier.name, { name: 'ptr' })
            }
            else if (statement instanceof quimblos.AssignStatement) {
                code.push(...this.compile_assign(statement.target, statement.source))
            }
            else if (statement instanceof quimblos.SleepStatement) {
                code.push(this._sleep(statement.time))
            }
            else if (statement instanceof quimblos.LogStatement) {
                const chunk = this.chunk_expression(statement.value);
                code.push(...chunk.prepare)
                code.push(this._log('_$_', chunk.out.type, chunk.out.value))
            }
        }

        return code;
    }

    private compile_assign(target: quimblos.Reference, source: quimblos.Expression) {
        const code: Instruction[] = [];

        const target_chunk = this.chunk_ptr(target);
        code.push(...target_chunk.prepare);

        const source_chunk = this.chunk_expression(source);
        code.push(...source_chunk.prepare);

        // let bind: keyof qasm.OpBind;
        // if (source_chunk.out.type.name === 'ptr') {
        //     if (target_chunk.out.type.name === 'ptr') bind = 'REF_REF';
        //     else bind = 'NODE_REF';
        // }
        // else {
        //     if (target_chunk.out.type.name === 'ptr') bind = 'REF_NODE';
        //     else bind = 'NODE_NODE';
        // }
        const bind = 'NODE_NODE';

        code.push({
            op: 'SET',
            bind,
            target: target_chunk.out.value as Ref,
            source: source_chunk.out
        })

        return code;
    }

    // Meta-Instructions

    private chunk_ptr(node: quimblos.Reference): Chunk {
        if (!node.index) {
            return {
                prepare: [],
                out: {
                    type: { name: 'ptr' },
                    value: { device: node.device, node: node.node }
                }
            }
        }
        throw new Error(`Indexed pointer not implemented yet`);
    }
    
    private chunk_expression(node: quimblos.Expression): Chunk {
        if (node.terms.length === 1) {
            const value = node.terms[0]!.value;
            if (value instanceof quimblos.Literal) {
                return {
                    prepare: [],
                    out: {
                        type: this.type_of_literal(value),
                        value: value.value
                    }
                }
            }
            throw new Error(`Non-literal values not implemented yet`);
        }

        const prepare: Instruction[] = []

        const parts: {
            left: number,
            right: number,
            op: quimblos.BoolOp | quimblos.MathOp
        }[] = [];

        for (let i=0; i < node.terms.length-1; i++) {
            if (!['*','/'].includes(node.ops[i]!)) continue;
            parts.push({ left: i, right: i+1, op: node.ops[i]! })
        }
        for (let i=0; i < node.terms.length-1; i++) {
            if (!['+','-'].includes(node.ops[i]!)) continue;
            parts.push({ left: i, right: i+1, op: node.ops[i]! })
        }
        
        if (parts.length < node.terms.length-1) {
            throw new Error(`Some expression operators not implemented yet`);
        }

        const targets: Record<number, Ref> = {};

        let expr_type;
        let target;

        for (const part of parts) {
            const left = node.terms[part.left]!.value;
            const right = node.terms[part.right]!.value;
            
            target = targets[part.left];
            if (!target) {
                if (left instanceof quimblos.Expression) {
                    throw new Error(`Nested expressions not implemented yet`);
                }
                else if (left instanceof quimblos.Reference) {
                    throw new Error(`Non-literal values not implemented yet`);
                }
                else { // if (left instanceof quimblos.Literal)
                    expr_type = this.type_of_literal(left);
                    target = { node: this.make_symbol(undefined, expr_type) };
                    prepare.push(this._add(target!, expr_type, left.value));
                }
            }

            let type: Type, value;
            type = { name: 'ptr' };
            value = targets[part.right];
            if (!value) {
                if (right instanceof quimblos.Expression) {
                    throw new Error(`Nested expressions not implemented yet`);
                }
                else if (right instanceof quimblos.Reference) {
                    throw new Error(`Non-literal values not implemented yet`);
                }
                else { // if (right instanceof quimblos.Literal)
                    type = this.type_of_literal(right);
                    value = right.value;
                }
            }
            targets[part.left] = target;
            targets[part.right] = target;
            console.log(part, targets, prepare);


            switch (part.op) {
                case "and": throw new Error(`Operator '${part.op}' not implemented yet`)
                case "or": throw new Error(`Operator '${part.op}' not implemented yet`)
                case "==": throw new Error(`Operator '${part.op}' not implemented yet`)
                case "!=": throw new Error(`Operator '${part.op}' not implemented yet`)
                case ">": throw new Error(`Operator '${part.op}' not implemented yet`)
                case "<": throw new Error(`Operator '${part.op}' not implemented yet`)
                case ">=": throw new Error(`Operator '${part.op}' not implemented yet`)
                case "<=": throw new Error(`Operator '${part.op}' not implemented yet`)
                case "+":
                    prepare.push(this._add(target!, type, value)); break;
                case "-": 
                    prepare.push(this._sub(target!, type, value)); break;
                case "*":
                    prepare.push(this._mult(target!, type, value)); break;
                case "/":
                    prepare.push(this._div(target!, type, value)); break;
                case "%": throw new Error(`Operator '${part.op}' not implemented yet`)
                case "^": throw new Error(`Operator '${part.op}' not implemented yet`)
            }
        }

        return {
            prepare,
            out: {
                type: { name: 'ptr' },
                value: target!
            }
        }
    }

    // Instructions

    private _use_device(name: string): Instruction {
        return { op: 'USE_DEVICE', name }
    }
    private _add(target: Ref, type: Type, value: Value|Ref): Instruction {
        return { op: 'ADD', target, source: { type, value } }
    }
    private _sub(target: Ref, type: Type, value: Value|Ref): Instruction {
        return { op: 'SUB', target, source: { type, value } }
    }
    private _mult(target: Ref, type: Type, value: Value|Ref): Instruction {
        return { op: 'MULT', target, source: { type, value } }
    }
    private _div(target: Ref, type: Type, value: Value|Ref): Instruction {
        return { op: 'DIV', target, source: { type, value } }
    }
    private _mod(target: Ref, type: Type, value: Value|Ref): Instruction {
        return { op: 'MOD', target, source: { type, value } }
    }
    private _pow(target: Ref, type: Type, value: Value|Ref): Instruction {
        return { op: 'POW', target, source: { type, value } }
    }
    private _sleep(time: number): Instruction {
        return { op: 'SLEEP', time }
    }
    private _log(device: string, typespec: TypeSpec, value: Value|Ref): Instruction {
        const type = this.type_from_spec(typespec);
        return { op: 'LOG', device, source: { type, value } }
    }

    // Internal Helpers

    private make_symbol(name: string|undefined, type: TypeSpec) {
        name ??= '_v_'+Object.keys(this.nodes).length;
        this.nodes[name] = { name, type: this.type_from_spec(type) };
        return name;
    }

    private type_from_spec(spec: TypeSpec): Type {
        if (spec.name === 'ptr') return { name: 'ptr' };
        if (spec.arr_length != null) return { name: 'arr', item: spec.name, length: spec.arr_length }
        else return { name: spec.name } as Type
    }

    private type_of_literal(node: quimblos.Literal): Type {
        switch (node.literal_type) {
            case "Boolean":
                return { name: 'bool' }
            case "Hexcode":
                // TODO
                return { name: 'u32' }
            case "Bitmask":
                return { name: 'u32' }
            case "Float":
                return { name: 'f32' }
            case "UnsignedInteger":
                return { name: 'u32' }
            case "Integer":
                return { name: 'i32' }
            case "String":
                return { name: 'str' }
        }
    }

}