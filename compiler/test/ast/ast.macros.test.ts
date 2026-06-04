import { quimblos } from "../../src/lang/semantics";
import * as lib from "../lib";

describe('[ast] UseDriverMacro', () => {
    it('#driver NAME', () => {
        const ast = lib.compile_ast(`
            #driver LED
        `);
        const script = ast.root as quimblos.Script;
        const node = script.macros[0] as quimblos.UseDriverMacro;
        expect(node).toBeInstanceOf(quimblos.UseDriverMacro)
        expect(node.device).toEqual('LED')
    })
})

describe('[ast] UseTopicMacro', () => {
    it('#topic NAME', () => {
        const ast = lib.compile_ast(`
            #topic log
        `);
        const script = ast.root as quimblos.Script;
        const node = script.macros[0] as quimblos.UseTopicMacro;
        expect(node).toBeInstanceOf(quimblos.UseTopicMacro)
        expect(node.topic).toEqual('log')
    })
})
