import { make_syntax_parser, AST } from "@quimblos/langmaker";
import { Node, QuimblosCompiler } from "../src/compiler";
import { Kernel } from "../src/kernel";
import { make_quimblos_linter } from "../src/lang/linter";
import { quimblos_semantics } from "../src/lang/semantics";
import { quimblos_syntax } from "../src/lang/syntax";

export const kernel: Kernel = {
    get_device(name: string) {
        return {
            name: 'LEDBAR',
            nodes: [{ name: 'on'}]
        }
    }
}

export const compile = (input: string) => {
    const syntax_parser = make_syntax_parser(quimblos_syntax);
    const ast = new AST(quimblos_semantics);
    const linter = make_quimblos_linter(kernel);

    const cst = syntax_parser(input);
    ast.update(cst);
    linter.lint(ast);

    const compiled = QuimblosCompiler.compile(kernel, ast);
    if (ast.errors.length) {
        throw new Error(`AST contains errors: ${ast.errors.map(e => e.error).join(',')}`)
    }
    return compiled;
}

export const node_v_0_ref: Node = { type: { name: 'ref' }, value: { node: '_v_0' }};
export const node_v_1_ref: Node = { type: { name: 'ref' }, value: { node: '_v_1' }};
export const node_v_2_ref: Node = { type: { name: 'ref' }, value: { node: '_v_2' }};
export const node_v_3_ref: Node = { type: { name: 'ref' }, value: { node: '_v_3' }};
export const node_v_4_ref: Node = { type: { name: 'ref' }, value: { node: '_v_4' }};
export const node_i32 = (v: number): Node => ({ type: { name: 'i32' }, value: v });
export const node_u32 = (v: number): Node => ({ type: { name: 'u32' }, value: v });
export const node_bool = (v: boolean): Node => ({ type: { name: 'bool' }, value: v });
export const node_str = (v: string): Node => ({ type: { name: 'str' }, value: v });
export const node_script_ref = (name: string, index?: Node): Node => ({ type: { name: 'ref' }, value: { device: undefined, node: name }, index });
