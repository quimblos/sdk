import * as lib from "./lib";

describe('AssignStatement', () => {
    it('x = 1', () => {
        const compiled = lib.compile(`
            var x:u8\n
            x = 1
        `);
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_script_ref('x'), source: lib.node_u32(1) }
        ])
    })
    it('x = \'test\'', () => {
        const compiled = lib.compile(`
            var x:str\n
            x = 'test'
        `);
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_script_ref('x'), source: lib.node_str('test') }
        ])
    })
})
describe('SleepStatement', () => {
    it('sleep 0', () => {
        const compiled = lib.compile(`
            sleep 0
        `);
        expect(compiled.code).toEqual([
            { op: 'SLEEP', time: lib.node_u32(0) }
        ])
    })
    it('sleep 1000', () => {
        const compiled = lib.compile(`
            sleep 1000
        `);
        expect(compiled.code).toEqual([
            { op: 'SLEEP', time: lib.node_u32(1000) }
        ])
    })
})
describe('LogStatement', () => {
    it('out true', () => {
        const compiled = lib.compile(`
            out true
        `);
        expect(compiled.code).toEqual([
            { op: 'LOG', device: '_$_', source: lib.node_bool(true) }
        ])
    })
    it('out 0', () => {
        const compiled = lib.compile(`
            out 0
        `);
        expect(compiled.code).toEqual([
            { op: 'LOG', device: '_$_', source: lib.node_u32(0) }
        ])
    })
    it('out \'test\'', () => {
        const compiled = lib.compile(`
            out 'test'
        `);
        expect(compiled.code).toEqual([
            { op: 'LOG', device: '_$_', source: lib.node_str('test') }
        ])
    })
    // it('out x', () => {
    //     const compiled = lib.compile(`
    //         var x:u8\n
    //         out x
    //     `);
    //     expect(compiled.code).toEqual([
    //         { op: 'LOG', device: '_$_', type: { name: 'u8' }, value: { node: 'x' } }
    //     ])
    // })
    
})
