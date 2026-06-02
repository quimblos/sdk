import { quimblos } from "../../src/lang/semantics";
import * as lib from "../lib";

describe('[ast] UseDriverMacro', () => {
    it('#driver NAME', () => {
        const ast = lib.compile_ast(`
            #driver LED
        `);
        const node = ast.root.children!()[0] as quimblos.UseDriverMacro;
        expect(node).toBeInstanceOf(quimblos.UseDriverMacro)
        expect(node.device).toEqual('LED')
    })
})

describe('[ast] UseTopicMacro', () => {
    it.only('#topic NAME', () => {
        const ast = lib.compile_ast(`
            #topic log
        `);
        const node = ast.root.children!()[0] as quimblos.UseTopicMacro;
        expect(node).toBeInstanceOf(quimblos.UseTopicMacro)
        expect(node.topic).toEqual('log')
    })
})
