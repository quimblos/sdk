import * as lib from "./lib";

describe('TypeDeclaration', () => {
    it('type Vec = [u8]', () => {
        const { script } = lib.compile_block_ast(`
            type Vec = [u8]
        `);
        expect(script.statements).toEqual({
            'Vec': { add: {
                kind: 'vec',
                children: [{ use: 'u8' }]
            }}
        })
    })
    it('type Map = {u8}', () => {
        const compiled = lib.compile(`
            type Map = {u8}
        `);
        expect(compiled.types).toEqual({
            'Map': { add: {
                kind: 'map',
                children: [{ use: 'u8' }]
            }}
        })
    })
    it('type Event = @u8', () => {
        const compiled = lib.compile(`
            type Event = @u8
        `);
        expect(compiled.types).toEqual({
            'Event': { add: {
                kind: 'event',
                children: [{ use: 'u8' }]
            }}
        })
    })
    it('type Struct = { a:u8, b:str }', () => {
        const compiled = lib.compile(`
            type Struct = { a:u8, b:str }
        `);
        expect(compiled.types).toEqual({
            'Struct': { add: {
                kind: 'struct',
                children: [
                    { key: 'a', use: 'u8' },
                    { key: 'b', use: 'str' }
                ]
            }}
        })
    })

    it('type VecOfVec = [[u8]]', () => {
        const compiled = lib.compile(`
            type VecOfVec = [[u8]]
        `);
        expect(compiled.types).toEqual({
            'VecOfVec': { add: {
                kind: 'vec',
                children: [
                    { add: {
                        kind: 'vec',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type VecOfMap = [{u8}]', () => {
        const compiled = lib.compile(`
            type VecOfMap = [{u8}]
        `);
        expect(compiled.types).toEqual({
            'VecOfMap': { add: {
                kind: 'vec',
                children: [
                    { add: {
                        kind: 'map',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type VecOfEvent = [@u8]', () => {
        const compiled = lib.compile(`
            type VecOfEvent = [@u8]
        `);
        expect(compiled.types).toEqual({
            'VecOfEvent': { add: {
                kind: 'vec',
                children: [
                    { add: {
                        kind: 'event',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type VecOfStruct = [{a:u8,b:str}]', () => {
        const compiled = lib.compile(`
            type VecOfStruct = [{a:u8,b:str}]
        `);
        expect(compiled.types).toEqual({
            'VecOfStruct': { add: {
                kind: 'vec',
                children: [
                    { add: {
                        kind: 'struct',
                        children: [
                            { key: 'a', use: 'u8' },
                            { key: 'b', use: 'str' },
                        ]
                    }}
                ]
            }}
        })
    })
    it('type Vec = [Map]', () => {
        const compiled = lib.compile(`
            type Map = {u8}\n
            type Vec = [Map]
        `);
        expect(compiled.types).toEqual({
            'Map': { add: {
                kind: 'map',
                children: [{ use: 'u8' }]
            }},
            'Vec': { add: {
                kind: 'vec',
                children: [{ use: 'Map' }]
            }}
        })
    })

    it('type MapOfVec = {[u8]}', () => {
        const compiled = lib.compile(`
            type MapOfVec = {[u8]}
        `);
        expect(compiled.types).toEqual({
            'MapOfVec': { add: {
                kind: 'map',
                children: [
                    { add: {
                        kind: 'vec',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type MapOfMap = {{u8}}', () => {
        const compiled = lib.compile(`
            type MapOfMap = {{u8}}
        `);
        expect(compiled.types).toEqual({
            'MapOfMap': { add: {
                kind: 'map',
                children: [
                    { add: {
                        kind: 'map',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type MapOfEvent = {@u8}', () => {
        const compiled = lib.compile(`
            type MapOfEvent = {@u8}
        `);
        expect(compiled.types).toEqual({
            'MapOfEvent': { add: {
                kind: 'map',
                children: [
                    { add: {
                        kind: 'event',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type MapOfStruct = {{a:u8,b:str}}', () => {
        const compiled = lib.compile(`
            type MapOfEvent = {{a:u8,b:str}}
        `);
        expect(compiled.types).toEqual({
            'MapOfEvent': { add: {
                kind: 'map',
                children: [
                    { add: {
                        kind: 'struct',
                        children: [
                            { key: 'a', use: 'u8' },
                            { key: 'b', use: 'str' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type Map = {Event}', () => {
        const compiled = lib.compile(`
            type Event = @u8\n
            type Map = {Event}
        `);
        expect(compiled.types).toEqual({
            'Event': { add: {
                kind: 'event',
                children: [
                    { use: 'u8' }
                ]
            }},
            'Map': { add: {
                kind: 'map',
                children: [
                    { use: 'Event' }
                ]
            }}
        })
    })

    it('type EventOfVec = @[u8]', () => {
        const compiled = lib.compile(`
            type EventOfVec = @[u8]
        `);
        expect(compiled.types).toEqual({
            'EventOfVec': { add: {
                kind: 'event',
                children: [
                    { add: {
                        kind: 'vec',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type EventOfMap = @{u8}', () => {
        const compiled = lib.compile(`
            type EventOfMap = @{u8}
        `);
        expect(compiled.types).toEqual({
            'EventOfMap': { add: {
                kind: 'event',
                children: [
                    { add: {
                        kind: 'map',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type EventOfEvent = @@u8', () => {
        const compiled = lib.compile(`
            type EventOfEvent = @@u8
        `);
        expect(compiled.types).toEqual({
            'EventOfEvent': { add: {
                kind: 'event',
                children: [
                    { add: {
                        kind: 'event',
                        children: [
                            { use: 'u8' },
                        ]
                    }}
                ]
            }}
        })
    })

    it('type EventOfStruct = @{a:u8,b:str}', () => {
        const compiled = lib.compile(`
            type EventOfEvent = @{a:u8,b:str}
        `);
        expect(compiled.types).toEqual({
            'EventOfEvent': { add: {
                kind: 'event',
                children: [
                    { add: {
                        kind: 'struct',
                        children: [
                            { key: 'a', use: 'u8' },
                            { key: 'b', use: 'str' },
                        ]
                    }}
                ]
            }}
        })
    })
    
    it('type Event = @Struct', () => {
        const compiled = lib.compile(`
            type Struct = {a:u8,b:str}\n
            type Event = @Struct
        `);
        expect(compiled.types).toEqual({
            'Struct': { add: {
                kind: 'struct',
                children: [
                    { key: 'a', use: 'u8' },
                    { key: 'b', use: 'str' }
                ]
            }},
            'Event': { add: {
                kind: 'event',
                children: [
                    { use: 'Struct' }
                ]
            }}
        })
    })
})

// describe('VariableDeclaration', () => {
//     it('var abc', () => {
//         const compiled = lib.compile(`
//             var abc
//         `);
//         expect(compiled.nodes).toEqual({
//             'abc': { name: 'abc' }
//         })
//     })
    
//     it('var abc:u8', () => {
//         const compiled = lib.compile(`
//             var abc:u8
//         `);
//         expect(compiled.nodes).toEqual({
//             'abc': { name: 'abc', type: { name: 'u8' }}
//         })
//     })
    
//     it('var abc:f32', () => {
//         const compiled = lib.compile(`
//             var abc:f32
//         `);
//         expect(compiled.nodes).toEqual({
//             'abc': { name: 'abc', type: { name: 'f32' }}
//         })
//     })

//     it('var abc:str', () => {
//         const compiled = lib.compile(`
//             var abc:str
//         `);
//         expect(compiled.nodes).toEqual({
//             'abc': { name: 'abc', type: { name: 'str' }}
//         })
//     })

//     it('var abc:u8[3]', () => {
//         const compiled = lib.compile(`
//             var abc:u8[3]
//         `);
//         expect(compiled.nodes).toEqual({
//             'abc': { name: 'abc', type: { name: 'vec', item: 'u8', length: 3 }}
//         })
//     })

//     it('var abc:f32[3]', () => {
//         const compiled = lib.compile(`
//             var abc:f32[3]
//         `);
//         expect(compiled.nodes).toEqual({
//             'abc': { name: 'abc', type: { name: 'vec', item: 'f32', length: 3 }}
//         })
//     })

//     it('var abc:str[3]', () => {
//         const compiled = lib.compile(`
//             var abc:str[3]
//         `);
//         expect(compiled.nodes).toEqual({
//             'abc': { name: 'abc', type: { name: 'vec', item: 'str', length: 3 }}
//         })
//     })
// })