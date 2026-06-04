import { AST } from "@quimblos/langmaker";
import { quimblos } from "../../src/lang/semantics";
import * as lib from "../lib";

describe('[ast] VarDeclaration', () => {

    it('var x', () => {
        const ast = lib.compile_ast(`
            var x
        `);
        const script = ast.root as quimblos.Script;
        const node = script.statements[0] as quimblos.VariableDeclaration;
        expect(node).toBeInstanceOf(quimblos.VariableDeclaration)
        expect(node.identifier.name).toEqual('x')
        expect(node.identifier.type).toBeUndefined()
        expect(node.value).toBeUndefined()
    })

    it('var x:u8', () => {
        const ast = lib.compile_ast(`
            var x:u8
        `);
        const script = ast.root as quimblos.Script;
        const node = script.statements[0] as quimblos.VariableDeclaration;
        expect(node).toBeInstanceOf(quimblos.VariableDeclaration)
        expect(node.identifier.name).toEqual('x')
        const type = node.identifier.type as quimblos.Identifier;
        expect(type).toBeInstanceOf(quimblos.Identifier)
        expect(type.name).toEqual('u8');
        expect(node.value).toBeUndefined()
    })

    it('var x:[u8]', () => {
        const ast = lib.compile_ast(`
            var x:[u8]
        `);
        const script = ast.root as quimblos.Script;
        const node = script.statements[0] as quimblos.VariableDeclaration;
        expect(node).toBeInstanceOf(quimblos.VariableDeclaration)
        expect(node.identifier.name).toEqual('x')
        const type = node.identifier.type as quimblos.MapType;
        expect(type).toBeInstanceOf(quimblos.MapType)
        expect(type.kind).toEqual('vec');
        const subtype = type.type as quimblos.Identifier;
        expect(subtype).toBeInstanceOf(quimblos.Identifier);
        expect(subtype.name).toEqual('u8');
        expect(node.value).toBeUndefined()
    })

    it('var x = y', () => {
        const ast = lib.compile_ast(`
            var x = y
        `);
        const script = ast.root as quimblos.Script;
        const node = script.statements[0] as quimblos.VariableDeclaration;
        expect(node).toBeInstanceOf(quimblos.VariableDeclaration)
        expect(node.identifier.name).toEqual('x')
        const value = node.value as quimblos.Expression;
        expect(value).toBeInstanceOf(quimblos.Expression);
    })

    it('var x:u8 = y', () => {
        const ast = lib.compile_ast(`
            var x:u8 = y
        `);
        const script = ast.root as quimblos.Script;
        const node = script.statements[0] as quimblos.VariableDeclaration;
        expect(node).toBeInstanceOf(quimblos.VariableDeclaration)
        expect(node.identifier.name).toEqual('x')
        const type = node.identifier.type as quimblos.Identifier;
        expect(type).toBeInstanceOf(quimblos.Identifier)
        expect(type.name).toEqual('u8');
        const value = node.value as quimblos.Expression;
        expect(value).toBeInstanceOf(quimblos.Expression);
    })

    it('var x:[u8] = y', () => {
        const ast = lib.compile_ast(`
            var x:[u8] = y
        `);
        const script = ast.root as quimblos.Script;
        const node = script.statements[0] as quimblos.VariableDeclaration;
        expect(node).toBeInstanceOf(quimblos.VariableDeclaration)
        expect(node.identifier.name).toEqual('x')
        const type = node.identifier.type as quimblos.MapType;
        expect(type).toBeInstanceOf(quimblos.MapType)
        expect(type.kind).toEqual('vec');
        const subtype = type.type as quimblos.Identifier;
        expect(subtype).toBeInstanceOf(quimblos.Identifier);
        expect(subtype.name).toEqual('u8');
        const value = node.value as quimblos.Expression;
        expect(value).toBeInstanceOf(quimblos.Expression);
    })

})
