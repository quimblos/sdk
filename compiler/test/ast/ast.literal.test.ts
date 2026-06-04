import { AST } from "@quimblos/langmaker";
import { quimblos } from "../../src/lang/semantics";
import * as lib from "../lib";

function check(ast: AST, type: string, value: any) {
    const script = ast.root as quimblos.Script;
    const node = script.statements[0] as quimblos.VariableDeclaration;
    const literal = node.value!.terms[0]!.value as quimblos.Literal;
    expect(literal).toBeInstanceOf(quimblos.Literal);
    expect(literal.literal_type).toEqual(type)
    expect(literal.value).toEqual(value)
}

describe('[ast] Literal', () => {

    it('var a = true', () => {
        const ast = lib.compile_ast(`
            var a = true
        `);
        check(ast, 'Boolean', true);
    })
    it('var a = false', () => {
        const ast = lib.compile_ast(`
            var a = false
        `);
        check(ast, 'Boolean', false);
    })

    it('var a = #123', () => {
        const ast = lib.compile_ast(`
            var a = #123
        `);
        check(ast, 'Hexcode', '#123');
    })
    it('var a = #123456', () => {
        const ast = lib.compile_ast(`
            var a = #123456
        `);
        check(ast, 'Hexcode', '#123456');
    })

    it('var a = 0b0101', () => {
        const ast = lib.compile_ast(`
            var a = 0b0101
        `);
        check(ast, 'Bitmask', '0b0101');
    })
    it('var a = 0b11110000', () => {
        const ast = lib.compile_ast(`
            var a = 0b11110000
        `);
        check(ast, 'Bitmask', '0b11110000');
    })

    it('var a = 12.', () => {
        const ast = lib.compile_ast(`
            var a = 12.
        `);
        check(ast, 'Float', 12);
    })
    it('var a = .34', () => {
        const ast = lib.compile_ast(`
            var a = .34
        `);
        check(ast, 'Float', .34);
    })
    it('var a = 12.34', () => {
        const ast = lib.compile_ast(`
            var a = 12.34
        `);
        check(ast, 'Float', 12.34);
    })

    it('var a = -123', () => {
        const ast = lib.compile_ast(`
            var a = -123
        `);
        check(ast, 'Integer', -123);
    })
    it('var a = 123', () => {
        const ast = lib.compile_ast(`
            var a = 123
        `);
        check(ast, 'UnsignedInteger', 123);
    })

    it('var a = \'\'', () => {
        const ast = lib.compile_ast(`
            var a = \'\'
        `);
        check(ast, 'String', '');
    })
    it('var a = \'test\'', () => {
        const ast = lib.compile_ast(`
            var a = \'test\'
        `);
        check(ast, 'String', 'test');
    })

})