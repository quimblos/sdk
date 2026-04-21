import * as lib from "./lib";

describe('Math', () => {
    it('1', () => {
        const compiled = lib.compile(`
            out 1
        `);
        expect(compiled.code).toEqual([
            { op: 'LOG', device: '_$_', source: lib.node_u32(1) }
        ])
    })
    it('1+2', () => {
        const compiled = lib.compile(`
            out 1+2
        `);
        
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(1) },
            { op: 'ADD', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('2-1', () => {
        const compiled = lib.compile(`
            out 2-1
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'SUB', target: lib.node_v_0_ref, source: lib.node_u32(1) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1+2-3', () => {
        const compiled = lib.compile(`
            out 1+2-3
        `);
        
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(1) },
            { op: 'ADD', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'SUB', target: lib.node_v_0_ref, source: lib.node_u32(3) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('-1-2+3+4', () => {
        const compiled = lib.compile(`
            out -1-2+3+4
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'i32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_i32(-1) },
            { op: 'SUB', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'ADD', target: lib.node_v_0_ref, source: lib.node_u32(3) },
            { op: 'ADD', target: lib.node_v_0_ref, source: lib.node_u32(4) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1*2', () => {
        const compiled = lib.compile(`
            out 1*2
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(1) },
            { op: 'MULT', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1*2/3', () => {
        const compiled = lib.compile(`
            out 1*2/3
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(1) },
            { op: 'MULT', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'DIV', target: lib.node_v_0_ref, source: lib.node_u32(3) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1*2+3*4', () => {
        const compiled = lib.compile(`
            out 1*2+3*4
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } },
            '_v_1': { name: '_v_1', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(1) },
            { op: 'MULT', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'SET', target: lib.node_v_1_ref, source: lib.node_u32(3) },
            { op: 'MULT', target: lib.node_v_1_ref, source: lib.node_u32(4) },
            { op: 'ADD', target: lib.node_v_0_ref, source: lib.node_v_1_ref },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1+2-3*4/5+6-7*8/9', () => {
        const compiled = lib.compile(`
            out 1+2-3*4/5+6-7*8/9
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } },
            '_v_1': { name: '_v_1', type: { name: 'u32' } },
            '_v_2': { name: '_v_2', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(3) },
            { op: 'MULT', target: lib.node_v_0_ref, source: lib.node_u32(4) },
            { op: 'DIV', target: lib.node_v_0_ref, source: lib.node_u32(5) },
            { op: 'SET', target: lib.node_v_1_ref, source: lib.node_u32(7) },
            { op: 'MULT', target: lib.node_v_1_ref, source: lib.node_u32(8) },
            { op: 'DIV', target: lib.node_v_1_ref, source: lib.node_u32(9) },
            { op: 'SET', target: lib.node_v_2_ref, source: lib.node_u32(1) },
            { op: 'ADD', target: lib.node_v_2_ref, source: lib.node_u32(2) },
            { op: 'SUB', target: lib.node_v_2_ref, source: lib.node_v_0_ref },
            { op: 'ADD', target: lib.node_v_2_ref, source: lib.node_u32(6) },
            { op: 'SUB', target: lib.node_v_2_ref, source: lib.node_v_1_ref },
            { op: 'LOG', device: '_$_', source: lib.node_v_2_ref }
        ])
    })
    it('1*2+3^4*5', () => {
        const compiled = lib.compile(`
            out 1*2+3^4*5
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } },
            '_v_1': { name: '_v_1', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(3) },
            { op: 'POW', target: lib.node_v_0_ref, source: lib.node_u32(4) },
            { op: 'SET', target: lib.node_v_1_ref, source: lib.node_u32(1) },
            { op: 'MULT', target: lib.node_v_1_ref, source: lib.node_u32(2) },
            { op: 'MULT', target: lib.node_v_0_ref, source: lib.node_u32(5) },
            { op: 'ADD', target: lib.node_v_1_ref, source: lib.node_v_0_ref },
            { op: 'LOG', device: '_$_', source: lib.node_v_1_ref }
        ])
    })
})
describe('Bool', () => {
    it('true and false', () => {
        const compiled = lib.compile(`
            out true and false
        `);
        
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'bool' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_bool(true) },
            { op: 'AND', target: lib.node_v_0_ref, source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('true or false', () => {
        const compiled = lib.compile(`
            out true or false
        `);
        
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'bool' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_bool(true) },
            { op: 'OR', target: lib.node_v_0_ref, source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('true xor false', () => {
        const compiled = lib.compile(`
            out true xor false
        `);
        
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'bool' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_bool(true) },
            { op: 'XOR', target: lib.node_v_0_ref, source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1 and 2 or 3 xor 4', () => {
        const compiled = lib.compile(`
            out 1 and 2 or 3 xor 4
        `);
        
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(1) },
            { op: 'AND', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'OR', target: lib.node_v_0_ref, source: lib.node_u32(3) },
            { op: 'XOR', target: lib.node_v_0_ref, source: lib.node_u32(4) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
})
describe('Comparison', () => {
    it('1 == 2', () => {
        const compiled = lib.compile(`
            out 1 == 2
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'bool' } },
        })
        expect(compiled.code).toEqual([
            { op: 'SET_IF_EQ', target: lib.node_v_0_ref, left: lib.node_u32(1), right: lib.node_u32(2), true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1 > 2', () => {
        const compiled = lib.compile(`
            out 1 > 2
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'bool' } },
        })
        expect(compiled.code).toEqual([
            { op: 'SET_IF_GT', target: lib.node_v_0_ref, left: lib.node_u32(1), right: lib.node_u32(2), true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1 > 2 > 3', () => {
        const compiled = lib.compile(`
            out 1 > 2 > 3
        `);
        
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'bool' } },
        })
        expect(compiled.code).toEqual([
            { op: 'SET_IF_GT', target: lib.node_v_0_ref, left: lib.node_u32(1), right: lib.node_u32(2), true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'SET_IF_GT', target: lib.node_v_0_ref, left: lib.node_v_0_ref, right: lib.node_u32(3), true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_0_ref }
        ])
    })
    it('1 > 2 + 3', () => {
        const compiled = lib.compile(`
            out 1 > 2 + 3
        `);
        
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } },
            '_v_1': { name: '_v_1', type: { name: 'bool' } },
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'ADD', target: lib.node_v_0_ref, source: lib.node_u32(3) },
            { op: 'SET_IF_GT', target: lib.node_v_1_ref, left: lib.node_u32(1), right: lib.node_v_0_ref, true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_1_ref }
        ])
    })
    it('1 >= 2 + 3 <= 4', () => {
        const compiled = lib.compile(`
            out 1 >= 2 + 3 <= 4
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } },
            '_v_1': { name: '_v_1', type: { name: 'bool' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'ADD', target: lib.node_v_0_ref, source: lib.node_u32(3) },
            { op: 'SET_IF_LT', target: lib.node_v_1_ref, left: lib.node_u32(1), right: lib.node_v_0_ref, true_source: lib.node_bool(false), false_source: lib.node_bool(true) },
            { op: 'SET_IF_GT', target: lib.node_v_1_ref, left: lib.node_v_1_ref, right: lib.node_u32(4), true_source: lib.node_bool(false), false_source: lib.node_bool(true) },
            { op: 'LOG', device: '_$_', source: lib.node_v_1_ref }
        ])
    })
    it('1 + 2 < 3 > 4 + 5', () => {
        const compiled = lib.compile(`
            out 1 + 2 < 3 > 4 + 5
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } },
            '_v_1': { name: '_v_1', type: { name: 'u32' } },
            '_v_2': { name: '_v_2', type: { name: 'bool' } }
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(1) },
            { op: 'ADD', target: lib.node_v_0_ref, source: lib.node_u32(2) },
            { op: 'SET', target: lib.node_v_1_ref, source: lib.node_u32(4) },
            { op: 'ADD', target: lib.node_v_1_ref, source: lib.node_u32(5) },
            { op: 'SET_IF_LT', target: lib.node_v_2_ref, left: lib.node_v_0_ref, right: lib.node_u32(3), true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'SET_IF_GT', target: lib.node_v_2_ref, left: lib.node_v_2_ref, right: lib.node_v_1_ref, true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_2_ref }
        ])
    })
})
describe('Mixed Expressions', () => {
    it('1 + 2 >= 3 and 4 * 5 - 6 == 7', () => {
        const compiled = lib.compile(`
            out 1 + 2 >= 3 and 4 * 5 - 6 == 7
        `);
        expect(compiled.nodes).toEqual({
            '_v_0': { name: '_v_0', type: { name: 'u32' } },
            '_v_1': { name: '_v_1', type: { name: 'u32' } },
            '_v_2': { name: '_v_2', type: { name: 'bool' } },
            '_v_3': { name: '_v_3', type: { name: 'bool' } },
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_0_ref, source: lib.node_u32(4) },
            { op: 'MULT', target: lib.node_v_0_ref, source: lib.node_u32(5) },
            { op: 'SET', target: lib.node_v_1_ref, source: lib.node_u32(1) },
            { op: 'ADD', target: lib.node_v_1_ref, source: lib.node_u32(2) },
            { op: 'SUB', target: lib.node_v_0_ref, source: lib.node_u32(6) },
            { op: 'SET_IF_LT', target: lib.node_v_2_ref, left: lib.node_v_1_ref, right: lib.node_u32(3), true_source: lib.node_bool(false), false_source: lib.node_bool(true) },
            { op: 'SET_IF_EQ', target: lib.node_v_3_ref, left: lib.node_v_0_ref, right: lib.node_u32(7), true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'AND', target: lib.node_v_2_ref, source: lib.node_v_3_ref },
            { op: 'LOG', device: '_$_', source: lib.node_v_2_ref }
        ])
    })
})
describe('References', () => {
    it('a', () => {
        const compiled = lib.compile(`
            var a:u8\n
            out a
        `);
        expect(compiled.nodes).toEqual({
            'a': { name: 'a', type: { name: 'u8' } },
        })
        expect(compiled.code).toEqual([
            { op: 'LOG', device: '_$_', source: lib.node_script_ref('a') }
        ])
    })
    it('a + 1', () => {
        const compiled = lib.compile(`
            var a:u8\n
            out a + 1
        `);
        expect(compiled.nodes).toEqual({
            'a': { name: 'a', type: { name: 'u8' } },
            '_v_1': { name: '_v_1', type: { name: 'u8' } },
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_1_ref, source: lib.node_script_ref('a') },
            { op: 'ADD', target: lib.node_v_1_ref, source: lib.node_u32(1) },
            { op: 'LOG', device: '_$_', source: lib.node_v_1_ref }
        ])
    })
    it('a > 1 * 2 + b', () => {
        const compiled = lib.compile(`
            var a:u8\n
            var b:u8\n
            out a > 1 * 2 + b
        `);
        expect(compiled.nodes).toEqual({
            'a': { name: 'a', type: { name: 'u8' } },
            'b': { name: 'b', type: { name: 'u8' } },
            '_v_2': { name: '_v_2', type: { name: 'u32' } },
            '_v_3': { name: '_v_3', type: { name: 'bool' } },
        })
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_2_ref, source: lib.node_u32(1) },
            { op: 'MULT', target: lib.node_v_2_ref, source: lib.node_u32(2) },
            { op: 'ADD', target: lib.node_v_2_ref, source: lib.node_script_ref('b') },
            { op: 'SET_IF_GT', target: lib.node_v_3_ref, left: lib.node_script_ref('a'), right: lib.node_v_2_ref, true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'LOG', device: '_$_', source: lib.node_v_3_ref }
        ])
    })
})