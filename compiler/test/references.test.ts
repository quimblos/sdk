import * as lib from "./lib";

describe('Indexed References', () => {
    it('a[2]', () => {
        const compiled = lib.compile(`
            var a:u8[3]\n
            out a[2]
        `);
        expect(compiled.code).toEqual([
            { op: 'LOG', device: '_$_', source: lib.node_script_ref('a', lib.node_u32(2)) }
        ])
    })
    it('a[i]', () => {
        const compiled = lib.compile(`
            var a:u8[3]\n
            var i:u8\n
            out a[i]
        `);
        expect(compiled.code).toEqual([
            { op: 'LOG', device: '_$_', source: lib.node_script_ref('a', lib.node_script_ref('i')) }
        ])
    })
    it('a[b[0]]', () => {
        const compiled = lib.compile(`
            var a:u8[3]\n
            var b:u8[2]\n
            out a[b[0]]
        `);
        expect(compiled.code).toEqual([
            { op: 'LOG', device: '_$_', source: lib.node_script_ref('a', lib.node_script_ref('b', lib.node_u32(0))) }
        ])
    })
})
