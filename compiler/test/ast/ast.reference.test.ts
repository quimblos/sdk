import { AST } from "@quimblos/langmaker";
import { quimblos } from "../../src/lang/semantics";
import * as lib from "../lib";

function check(ast: AST, type: string, value: any) {
    const script = ast.root as quimblos.Script;
    const node = script.statements[0] as quimblos.VariableDeclaration;
    const ref = node.value!.terms[0]!.value as quimblos.Reference;
    expect(ref).toBeInstanceOf(quimblos.Reference);
    // expect(ref.).toEqual(type)
    // expect(literal.value).toEqual(value)
}

describe('[ast] Literal', () => {

    it('var a = x', () => {
        const ast = lib.compile_ast(`
            var a = x
        `);
        check(ast, 'Boolean', true);
    })

})