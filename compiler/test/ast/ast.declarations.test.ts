import { AST } from "@quimblos/langmaker";
import { quimblos } from "../../src/lang/semantics";
import * as lib from "../lib";

function test_type_declaration(ast: AST, name: string, kind: string, subtype_name: string) {
    // console.log(ast.root.cst.children[0].children[1].children[0]?.children[0]);
    const script = ast.root as quimblos.Script;
    const node = script.statements[0] as quimblos.TypeDeclaration;
    expect(node).toBeInstanceOf(quimblos.TypeDeclaration)
    expect(node.identifier.name).toEqual(name)
    const type = node.type as quimblos.MapType;
    expect(type).toBeInstanceOf(quimblos.MapType)
    expect(type.kind).toEqual(kind);
    const subtype = type.type as quimblos.Identifier;
    expect(subtype).toBeInstanceOf(quimblos.Identifier);
    expect(subtype.name).toEqual(subtype_name);
}

describe('[ast] TypeDeclaration', () => {
    it('type Vec = [u8]', () => {
        const ast = lib.compile_ast(`
            type Vec = [u8]
        `);
        test_type_declaration(ast, 'Vec', 'vec', 'u8');
    })
    it('type Map = {u8}', () => {
        const ast = lib.compile_ast(`
            type Map = {u8}
        `);
        test_type_declaration(ast, 'Map', 'map', 'u8');
    })
})
