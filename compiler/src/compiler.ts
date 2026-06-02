import { AST } from "@quimblos/langmaker";
import { quimblos } from "./lang/semantics";
import { Kernel } from "./kernel";
import { TypeDef } from "./lang/types";

export type TypeName = quimblos.TypeIdentifier['name']

export type Value = boolean|number|string|boolean[]|number[]|string[]
export type Ref = {
    device: string
    node: string
}

export type Node = {
    type: Exclude<Type, { name: 'ref' }>
    value: Value
} | {
    type: Extract<Type, { name: 'ref' }>
    value: Ref
    index?: Node | undefined
}

export type CodeAddr = [string, number] // name, offset

export type Instruction = 
    { $addr?: string, op: 'USE_DRIVER', name: string }
    | { $addr?: string, op: 'USE_TOPIC', name: string }
    | { $addr?: string, op: 'USE_VAR', name: string, type: Type }

    | { $addr?: string, op: 'SET', target: Node, source: Node }
    | { $addr?: string, op: 'HOLD', device: string }
    | { $addr?: string, op: 'RELEASE', device: string }

    | { $addr?: string, op: 'GOTO', code_addr: CodeAddr }
    | { $addr?: string, op: 'BRANCH', source: Node, true_addr: CodeAddr, false_addr: CodeAddr }
    
    | { $addr?: string, op: 'SET_IF_EQ', target: Node, left: Node, right: Node, true_source: Node, false_source: Node }
    | { $addr?: string, op: 'SET_IF_LT', target: Node, left: Node, right: Node, true_source: Node, false_source: Node }
    | { $addr?: string, op: 'SET_IF_GT', target: Node, left: Node, right: Node, true_source: Node, false_source: Node }

    | { $addr?: string, op: 'NOT', target: Node, source: Node }
    | { $addr?: string, op: 'AND', target: Node, source: Node }
    | { $addr?: string, op: 'OR', target: Node, source: Node }

    | { $addr?: string, op: 'ADD', target: Node, source: Node }
    | { $addr?: string, op: 'SUB', target: Node, source: Node }
    | { $addr?: string, op: 'MULT', target: Node, source: Node }
    | { $addr?: string, op: 'DIV', target: Node, source: Node }
    | { $addr?: string, op: 'POW', target: Node, source: Node }
    | { $addr?: string, op: 'MOD', target: Node, source: Node }
    
    | { $addr?: string, op: 'SLEEP', time: Node }
    | { $addr?: string, op: 'LOG', source: Node }

    | { $addr?: string, op: 'RETURN', device: string, source: Node }
    | { $addr?: string, op: 'RESET' }
    | { $addr?: string, op: 'STOP' }


export type NodeSpec = {
    type: Type
    name: string
}

type Chunk = {
    prepare: Instruction[]
    out: Node
}

export class QuimblosCompiler {

    private root: quimblos.Script;
    private nodes: Record<string, NodeSpec> = {};
    private code_addresses = 0;

    private macros: Instruction[] = [];
    private code: Instruction[] = [];
    private types: {
        [name: string]: TypeDef
    } = {}

    private constructor(
        public ast: AST
    ) {
        this.root = ast.root as quimblos.Script;
    }

    public static compile(kernel: Kernel, ast: AST) {
        const compiler = new QuimblosCompiler(ast);
        return compiler.compile();
    }

    public compile() {
        this.code = [];
        for (const node of this.root.macros) {
            if (node instanceof quimblos.UseDriverMacro) {
                this.macros.push(this._use_driver(node.device))
            }
            else if (node instanceof quimblos.UseTopicMacro) {
                this.macros.push(this._use_topic(node.topic))
            }
        }
        for (const block of this.root.blocks) {
            this.code.push(...this.compile_block(block));
        }
        return {
            macros: this.macros,
            nodes: this.nodes,
            code: this.code,
            types: this.types
        }
    }

    private compile_block(node: quimblos.Block): Instruction[] {
        const code: Instruction[] = [];

        for (const statement of node.statements) {
            if (statement instanceof quimblos.TypeDeclaration) {
                this.make_type(statement.identifier.name, statement.type)
            }
            if (statement instanceof quimblos.VariableDeclaration) {
                const type = this.type_from_identifier(statement.identifier.type);
                this.make_var(statement.identifier.name, type)
            }
            else if (statement instanceof quimblos.AssignStatement) {
                code.push(...this.compile_assign(statement.target, statement.source))
            }
            else if (statement instanceof quimblos.IfStatement) {
                if (statement.block) {
                    code.push(...this.compile_if(statement.expression, statement.block))
                }
            }
            else if (statement instanceof quimblos.WhileStatement) {
                if (statement.block) {
                    code.push(...this.compile_while(statement.expression, statement.block))
                }
            }
            else if (statement instanceof quimblos.SleepStatement) {
                code.push(...this.compile_sleep(statement.time))
            }
            else if (statement instanceof quimblos.LogStatement) {
                code.push(...this.compile_log('_$_', statement.value))
            }
        }

        return code;
    }

    private compile_assign(target: quimblos.Reference, source: quimblos.Expression) {
        const code: Instruction[] = [];

        const target_chunk = this.chunk_ref(target);
        code.push(...target_chunk.prepare);

        const source_chunk = this.chunk_expression(source);
        code.push(...source_chunk.prepare);

        code.push({
            op: 'SET',
            target: {
                type: { name: 'ref' },
                value: target_chunk.out.value as Ref
            },
            source: source_chunk.out
        })

        return code;
    }

    private compile_if(source: quimblos.Expression, block: quimblos.Block) {
        const code: Instruction[] = [];

        const source_chunk = this.chunk_expression(source);
        code.push(...source_chunk.prepare);
        
        const block_code = this.compile_block(block);

        const true_addr = this.make_code_addr();
        const false_addr = block_code.length === 1
            ? [true_addr[0], 1] as CodeAddr
            : this.make_code_addr(1);

        block_code.at(0)!.$addr = true_addr[0];
        block_code.at(-1)!.$addr = false_addr[0];

        code.push({
            op: 'BRANCH',
            source: source_chunk.out,
            true_addr,
            false_addr
        })
        code.push(...block_code);      

        return code;
    }

    private compile_while(source: quimblos.Expression, block: quimblos.Block) {
        const code: Instruction[] = [];

        const source_chunk = this.chunk_expression(source);
        code.push(...source_chunk.prepare);
        
        const block_code = this.compile_block(block);

        const start_addr = this.make_code_addr();
        const block_start_addr = this.make_code_addr();
        const end_addr = block_code.length === 1
            ? [block_start_addr[0], 1] as CodeAddr
            : this.make_code_addr(1);

        block_code.at(0)!.$addr = block_start_addr[0];

        code.push({
            op: 'BRANCH',
            source: source_chunk.out,
            true_addr: block_start_addr,
            false_addr: end_addr
        })        
        code.push(...block_code);

        code.at(0)!.$addr = start_addr[0];
        code.push({
            $addr: end_addr[0],
            op: 'GOTO',
            code_addr: start_addr
        })

        return code;
    }

    private compile_sleep(source: quimblos.Expression) {
        const code: Instruction[] = [];

        const source_chunk = this.chunk_expression(source);
        code.push(...source_chunk.prepare);

        code.push({
            op: 'SLEEP',
            time: source_chunk.out
        })

        return code;
    }

    private compile_log(device: string, source: quimblos.Expression) {
        const code: Instruction[] = [];

        const source_chunk = this.chunk_expression(source);
        code.push(...source_chunk.prepare);

        code.push({
            op: 'LOG',
            // device,
            source: source_chunk.out
        })

        return code;
    }

    // Meta-Instructions

    private chunk_ref(node: quimblos.Reference): Chunk {
        if (!node.index) {
            return {
                prepare: [],
                out: {
                    type: { name: 'ref' },
                    value: { device: node.device ?? '_$_', node: node.node }
                }
            }
        }
        const index = this.chunk_expression(node.index);
        return {
            prepare: index.prepare,
            out: {
                type: { name: 'ref' },
                value: { device: node.device ?? '_$_', node: node.node },
                index: index.out
            }
        }
    }
    
    private chunk_expression(node: quimblos.Expression): Chunk {
        if (node.terms.length === 1) {
            const value = node.terms[0]!.value;
            return this.chunk_value(value);
        }

        const prepare: Instruction[] = []
        let target: Node|undefined = undefined;

        const parts: {
            left: number,
            right: number,
            op: quimblos.BoolOp | quimblos.MathOp
        }[] = [];
        
        const sources: Record<number, Node> = {};
        const resolved = new Set<number>();

        const scan_terms = (...operators: string[]) => {
            for (let i=0; i < node.terms.length-1; i++) {
                if (!operators.includes(node.ops[i]!)) continue;
                const left = this.chunk_value(node.terms[i]!.value);
                const right = this.chunk_value(node.terms[i+1]!.value);
                prepare.push(...left.prepare);
                prepare.push(...right.prepare);
                parts.push({ left: i, right: i+1, op: node.ops[i]! })
                sources[i] = left.out;
                sources[i+1] = right.out;
            }
        }

        scan_terms('^');
        scan_terms('*','/');
        scan_terms('+','-');
        scan_terms('%');
        scan_terms('==','!=','>','<','>=','<=');
        scan_terms('and','or','xor');
        
        if (parts.length < node.terms.length-1) {
            throw new Error(`Some expression operators not implemented yet`);
        }

        for (let i = 0; i < parts.length; i++) {
            const part = parts[i]!;
            const is_comparison = ['==','!=','>','<','>=','<='].includes(part.op);

            const left = sources[part.left]!;
            const right = sources[part.right]!;

            let bool_target: Node|undefined = undefined;
            let bool_value: Ref|undefined = undefined;
            if (is_comparison) {
                if (left.type.name === 'ref') {
                    const node = this.nodes[(left.value as Ref).node]!;
                    if (resolved.has(part.left) && node.type.name === 'bool')
                        bool_value = left.value as Ref;
                    else
                        bool_value = this.make_var(undefined, { name: 'bool' });
                }
                else bool_value = this.make_var(undefined, { name: 'bool' });
                
                bool_target = bool_value ? { type: { name: 'ref' as const }, value: bool_value } : undefined;
                target = left
            }
            else {
                let value;
                if (left.type.name === 'ref') {
                    if (resolved.has(part.left)) value = left.value;
                    else {
                        const node = this.nodes[(left.value as Ref).node]!;
                        value = this.make_var(undefined, node.type);
                    }
                }
                else value = this.make_var(undefined, left.type);

                target = { type: { name: 'ref' }, value: value as Ref };
            }

            if (!resolved.has(part.left) && !is_comparison) {
                prepare.push(this._set(target, left));
            }

            const _true: Node = { type: { name: 'bool' }, value: true };
            const _false: Node = { type: { name: 'bool' }, value: false };

            switch (part.op) {
                case "not":
                    prepare.push(this._not(target, right)); break;
                case "and":
                    prepare.push(this._and(target, right)); break;
                case "or":
                    prepare.push(this._or(target, right)); break;
                // case "xor":
                //     prepare.push(this._xor(target, right)); break;
                case "==":
                    prepare.push(this._set_eq(bool_target!, target!, right, _true, _false)); break;
                case "!=":
                    prepare.push(this._set_eq(bool_target!, target!, right, _false, _true)); break;
                case ">":
                    prepare.push(this._set_gt(bool_target!, target!, right, _true, _false)); break;
                case "<":
                    prepare.push(this._set_lt(bool_target!, target!, right, _true, _false)); break;
                case "<=":
                    prepare.push(this._set_gt(bool_target!, target!, right, _false, _true)); break;
                case ">=":
                    prepare.push(this._set_lt(bool_target!, target!, right, _false, _true)); break;
                case "+":
                    prepare.push(this._add(target, right)); break;
                case "-": 
                    prepare.push(this._sub(target, right)); break;
                case "*":
                    prepare.push(this._mult(target, right)); break;
                case "/":
                    prepare.push(this._div(target, right)); break;
                case "%":
                    prepare.push(this._mod(target, right)); break;
                case "^":
                    prepare.push(this._pow(target, right)); break;
            }

            target = bool_target ?? target;

            sources[part.left] = target;
            for (let j = i+1; j < parts.length; j++) {
                if (parts[j]!.left === part.right) {
                    parts[j]!.left = part.left;
                }
            }
            
            resolved.add(part.left)
            resolved.add(part.right)
        }

        return {
            prepare,
            out: target!
        }
    }

    private chunk_value(node: quimblos.Literal | quimblos.Reference | quimblos.Expression): Chunk {
        if (node instanceof quimblos.Literal) {
            return {
                prepare: [],
                out: {
                    type: this.type_of_literal(node),
                    value: node.value as Value
                }
            }
        }
        else if (node instanceof quimblos.Expression) {
            return this.chunk_expression(node)
        }
        else {
            return this.chunk_ref(node)
        }
    }

    // Instructions

    private _use_driver(name: string): Instruction {
        return { op: 'USE_DRIVER', name }
    }
    private _use_topic(name: string): Instruction {
        return { op: 'USE_TOPIC', name }
    }
    private _use_var(name: string, type: Type): Instruction {
        return { op: 'USE_VAR', name, type }
    }
    private _set(target: Node, source: Node): Instruction {
        return { op: 'SET', target, source }
    }
    private _not(target: Node, source: Node): Instruction {
        return { op: 'NOT', target, source }
    }
    private _and(target: Node, source: Node): Instruction {
        return { op: 'AND', target, source }
    }
    private _or(target: Node, source: Node): Instruction {
        return { op: 'OR', target, source }
    }
    private _set_eq(target: Node, left: Node, right: Node, source: Node, source_else: Node): Instruction {
        return { op: 'SET_IF_EQ', target, left, right, true_source: source, false_source: source_else }
    }
    private _set_gt(target: Node, left: Node, right: Node, source: Node, source_else: Node): Instruction {
        return { op: 'SET_IF_GT', target, left, right, true_source: source, false_source: source_else }
    }
    private _set_lt(target: Node, left: Node, right: Node, source: Node, source_else: Node): Instruction {
        return { op: 'SET_IF_LT', target, left, right, true_source: source, false_source: source_else }
    }
    private _add(target: Node, source: Node): Instruction {
        return { op: 'ADD', target, source }
    }
    private _sub(target: Node, source: Node): Instruction {
        return { op: 'SUB', target, source }
    }
    private _mult(target: Node, source: Node): Instruction {
        return { op: 'MULT', target, source }
    }
    private _div(target: Node, source: Node): Instruction {
        return { op: 'DIV', target, source }
    }
    private _mod(target: Node, source: Node): Instruction {
        return { op: 'MOD', target, source }
    }
    private _pow(target: Node, source: Node): Instruction {
        return { op: 'POW', target, source }
    }
    private _log(device: string, source: Node): Instruction {
        return { op: 'LOG'/*, device*/, source }
    }

    // Internal Helpers

    private make_type(name: string|undefined, type: quimblos.Identifier | quimblos.Type): string {
        name ??= '_t_'+Object.keys(this.types).length;
        
        const make_def = (type: quimblos.Identifier | quimblos.Type): TypeDef => {
            if (type instanceof quimblos.Identifier) {
                return {
                    use: type.name,
                };
            }
            else if (type instanceof quimblos.MapType) {
                return {
                    add: {
                        kind: type.kind,
                        children: [make_def(type.type)]
                    }
                }
            }
            else if (type instanceof quimblos.StructType) {
                return {
                    add: {
                        kind: 'struct',
                        children: type.fields.map(field =>
                            ({ key: field.key, ...make_def(field.type) })
                        )
                    }
                }
            }
        }
        this.types[name] = make_def(type);

        return name;
    }
    private make_var(name: string|undefined, type: Type): Ref {
        name ??= '_v_'+Object.keys(this.nodes).length;
        this.nodes[name] = { name, type };
        return { device: '_$_', node: name };
    }
    private make_code_addr(offset = 0): CodeAddr {
        return [`_${this.code_addresses++}`, offset];
    }

    private type_from_identifier(node: quimblos.TypeIdentifier): Type {
        if (node.arr_length != null) return { name: 'vec', item: node.name, length: node.arr_length }
        else return { name: node.name } as Type
    }

    private type_of_literal(node: quimblos.Literal) {
        switch (node.literal_type) {
            case "Boolean":
                return { name: 'bool' as const }
            case "Hexcode":
                // TODO
                return { name: 'u32' as const }
            case "Bitmask":
                return { name: 'u32' as const }
            case "Float":
                return { name: 'f32' as const }
            case "UnsignedInteger":
                if (node.value <= 0xFF) return { name: 'u8' as const }
                else if (node.value <= 0xFFFF) return { name: 'u16' as const }
                else return { name: 'u32' as const }
            case "Integer":
                if (Math.abs(node.value) <= 0xFF) return { name: 'i8' as const }
                else if (Math.abs(node.value) <= 0xFFFF) return { name: 'i16' as const }
                else return { name: 'i32' as const }
            case "String":
                return { name: 'str' as const }
        }
    }

}