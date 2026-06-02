import { make_syntax_parser, AST } from "@quimblos/langmaker";
import { Node, QuimblosCompiler } from "../src/compiler";
import { Kernel } from "../src/kernel";
import { make_quimblos_linter } from "../src/lang/linter";
import { quimblos, quimblos_semantics } from "../src/lang/semantics";
import { quimblos_syntax } from "../src/lang/syntax";
import { quimblos_block_transformer, quimblos_identation_transformer } from "../src/lang/transformers";

export const kernel: Kernel = {
    get_driver(name: string) {
        return {
            name: 'LED8',
            variables: [{ name: 'on'}]
        }
    }
}

export const compile_cst = (input: string) => {
    const syntax_parser = make_syntax_parser(quimblos_syntax);
    const cst = syntax_parser(input);
    return cst;
}

export const compile_ast = (input: string) => {
    const cst = compile_cst(input);
    const ast = new AST(quimblos_semantics);
    ast.update(cst);
    return ast;
}

export const compile_idented_ast = (input: string) => {
    const ast = compile_ast(input);
    quimblos_identation_transformer.transform(ast);
    if (ast.errors.length) {
        throw new Error(`AST contains identation errors: ${ast.errors.map(e => e.error).join(',')}`)
    }
    return ast;
}

export const compile_block_ast = (input: string) => {
    const ast = compile_idented_ast(input);
    quimblos_block_transformer.transform(ast);
    if (ast.errors.length) {
        throw new Error(`AST contains block errors: ${ast.errors.map(e => e.error).join(',')}`)
    }
    return { ast, script: ast.root as quimblos.Script };
}

export const compile_linted_ast = (input: string) => {
    const { ast, script } = compile_block_ast(input)
    const linter = make_quimblos_linter(kernel);

    linter.lint(ast);
    if (ast.errors.length) {
        throw new Error(`AST contains lint errors: ${ast.errors.map(e => e.error).join(',')}`)
    }
    return { ast, script };
}

export const compile = (input: string) => {
    const { ast, script } = compile_linted_ast(input)

    const compiled = QuimblosCompiler.compile(kernel, ast);
    if (ast.errors.length) {
        throw new Error(`AST contains compilation errors: ${ast.errors.map(e => e.error).join(',')}`)
    }
    return compiled;
}

// export const node_v_0_ref: Node = { type: { name: 'ref' }, value: { node: '_v_0' }};
// export const node_v_1_ref: Node = { type: { name: 'ref' }, value: { node: '_v_1' }};
// export const node_v_2_ref: Node = { type: { name: 'ref' }, value: { node: '_v_2' }};
// export const node_v_3_ref: Node = { type: { name: 'ref' }, value: { node: '_v_3' }};
// export const node_v_4_ref: Node = { type: { name: 'ref' }, value: { node: '_v_4' }};
// export const node_i32 = (v: number): Node => ({ type: { name: 'i32' }, value: v });
// export const node_u32 = (v: number): Node => ({ type: { name: 'u32' }, value: v });
// export const node_bool = (v: boolean): Node => ({ type: { name: 'bool' }, value: v });
// export const node_str = (v: string): Node => ({ type: { name: 'str' }, value: v });
// export const node_script_ref = (name: string, index?: Node): Node => ({ type: { name: 'ref' }, value: { device: undefined, node: name }, index });
