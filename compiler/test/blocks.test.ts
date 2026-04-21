import * as lib from "./lib";

describe('If Statement', () => {
    it('if true (block with 1 statement)', () => {
        const compiled = lib.compile(`
            if true:\n
                out 'si'\n
            out 'whatever'
        `);
        expect(compiled.code).toEqual([
            { op: 'BRANCH', source: lib.node_bool(true), true_addr: ['_0',0], false_addr: ['_0',1] },
            { $addr: '_0', op: 'LOG', device: '_$_', source: lib.node_str('si') },
            { op: 'LOG', device: '_$_', source: lib.node_str('whatever') },
        ])
    })
    it('if true (block with 2 statements)', () => {
        const compiled = lib.compile(`
            if true:\n
                out 'do'\n
                out 're'\n
            out 'whatever'
        `);
        expect(compiled.code).toEqual([
            { op: 'BRANCH', source: lib.node_bool(true), true_addr: ['_0',0], false_addr: ['_1',1] },
            { $addr: '_0', op: 'LOG', device: '_$_', source: lib.node_str('do') },
            { $addr: '_1', op: 'LOG', device: '_$_', source: lib.node_str('re') },
            { op: 'LOG', device: '_$_', source: lib.node_str('whatever') },
        ])
    })
    it('if true (block with n statements)', () => {
        const compiled = lib.compile(`
            if true:\n
                out 'do'\n
                out 're'\n
                out 'mi'\n
                out 'fa'\n
            out 'whatever'
        `);
        expect(compiled.code).toEqual([
            { op: 'BRANCH', source: lib.node_bool(true), true_addr: ['_0',0], false_addr: ['_1',1] },
            { $addr: '_0', op: 'LOG', device: '_$_', source: lib.node_str('do') },
            { $addr: undefined, op: 'LOG', device: '_$_', source: lib.node_str('re') },
            { $addr: undefined, op: 'LOG', device: '_$_', source: lib.node_str('mi') },
            { $addr: '_1', op: 'LOG', device: '_$_', source: lib.node_str('fa') },
            { op: 'LOG', device: '_$_', source: lib.node_str('whatever') },
        ])
    })
    it('if a > 3', () => {
        const compiled = lib.compile(`
            var a:u8\n
            if a > 3:\n
                out 'si'\n
            out 'whatever'
        `);
        expect(compiled.code).toEqual([
            { op: 'SET_IF_GT', target: lib.node_v_1_ref, left: lib.node_script_ref('a'), right: lib.node_u32(3), true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'BRANCH', source: lib.node_v_1_ref, true_addr: ['_0',0], false_addr: ['_0',1] },
            { $addr: '_0', op: 'LOG', device: '_$_', source: lib.node_str('si') },
            { op: 'LOG', device: '_$_', source: lib.node_str('whatever') },
        ])
    })
    it('if 1 + a == 2 * b', () => {
        const compiled = lib.compile(`
            var a:u8\n
            var b:u8\n
            if 1 + a == 2 * b:\n
                out 'si'\n
            out 'whatever'
        `);
        expect(compiled.code).toEqual([
            { op: 'SET', target: lib.node_v_2_ref, source: lib.node_u32(2) },
            { op: 'MULT', target: lib.node_v_2_ref, source: lib.node_script_ref('b') },
            { op: 'SET', target: lib.node_v_3_ref, source: lib.node_u32(1) },
            { op: 'ADD', target: lib.node_v_3_ref, source: lib.node_script_ref('a') },
            { op: 'SET_IF_EQ', target: lib.node_v_4_ref, left: lib.node_v_3_ref, right: lib.node_v_2_ref, true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'BRANCH', source: lib.node_v_4_ref, true_addr: ['_0',0], false_addr: ['_0',1] },
            { $addr: '_0', op: 'LOG', device: '_$_', source: lib.node_str('si') },
            { op: 'LOG', device: '_$_', source: lib.node_str('whatever') },
        ])
    })
})

describe('While Statement', () => {
    it('while i < 3', () => {
        const compiled = lib.compile(`
            var i:u8
            while i < 3:\n
                out i\n
                i = i + 1\n
        `);
        expect(compiled.code).toEqual([
            { $addr: '_0', op: 'SET_IF_LT', target: lib.node_v_1_ref, left: lib.node_script_ref('i'), right: lib.node_u32(3), true_source: lib.node_bool(true), false_source: lib.node_bool(false) },
            { op: 'BRANCH', source: lib.node_v_1_ref, true_addr: ['_1',0], false_addr: ['_2',1] },
            { $addr: '_1', op: 'LOG', device: '_$_', source: lib.node_script_ref('i') },
            { op: 'SET', target: lib.node_v_2_ref, source: lib.node_script_ref('i') },
            { op: 'ADD', target: lib.node_v_2_ref, source: lib.node_u32(1) },
            { op: 'SET', target: lib.node_script_ref('i'), source: lib.node_v_2_ref },
            { $addr: '_2', op: 'GOTO', code_addr: ['_0', 0] },
        ])
    })
})