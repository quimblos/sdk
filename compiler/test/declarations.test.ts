import * as lib from "./lib";

describe('VariableDeclaration', () => {
    it('var abc:u8', () => {
        const compiled = lib.compile(`
            var abc:u8
        `);
        expect(compiled.nodes).toEqual({
            'abc': { name: 'abc', type: { name: 'u8' }}
        })
    })
    
    it('var abc:f32', () => {
        const compiled = lib.compile(`
            var abc:f32
        `);
        expect(compiled.nodes).toEqual({
            'abc': { name: 'abc', type: { name: 'f32' }}
        })
    })

    it('var abc:str', () => {
        const compiled = lib.compile(`
            var abc:str
        `);
        expect(compiled.nodes).toEqual({
            'abc': { name: 'abc', type: { name: 'str' }}
        })
    })

    it('var abc:u8[3]', () => {
        const compiled = lib.compile(`
            var abc:u8[3]
        `);
        expect(compiled.nodes).toEqual({
            'abc': { name: 'abc', type: { name: 'arr', item: 'u8', length: 3 }}
        })
    })

    it('var abc:f32[3]', () => {
        const compiled = lib.compile(`
            var abc:f32[3]
        `);
        expect(compiled.nodes).toEqual({
            'abc': { name: 'abc', type: { name: 'arr', item: 'f32', length: 3 }}
        })
    })

    it('var abc:str[3]', () => {
        const compiled = lib.compile(`
            var abc:str[3]
        `);
        expect(compiled.nodes).toEqual({
            'abc': { name: 'abc', type: { name: 'arr', item: 'str', length: 3 }}
        })
    })
})