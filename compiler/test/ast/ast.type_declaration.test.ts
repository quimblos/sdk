import { AST } from "@quimblos/langmaker";
import { quimblos } from "../../src/lang/semantics";
import * as lib from "../lib";

function check_name(ast: AST, name: string) {
    const script = ast.root as quimblos.Script;
    const node = script.statements[0] as quimblos.TypeDeclaration;
    expect(node).toBeInstanceOf(quimblos.TypeDeclaration)
    expect(node.identifier.name).toEqual(name)
    return node;
}

type TestTypeDef = string | {
    vec: TestTypeDef
} | {
    map: TestTypeDef
} | {
    event: TestTypeDef
} | {
    struct: Record<string, TestTypeDef>
}

function check_type(type: quimblos.Identifier | quimblos.Type, def: TestTypeDef) {
    if (typeof def === 'string') {
        const identifier = type as quimblos.Identifier;
        expect(identifier).toBeInstanceOf(quimblos.Identifier);
        expect(identifier.name).toEqual(def);
        return
    }
    if ('struct' in def) {
        const struct = type as quimblos.StructType;
        expect(struct).toBeInstanceOf(quimblos.StructType)
        expect(struct.fields).toHaveLength(Object.keys(def.struct).length);
        Object.keys(def.struct).forEach((key, i) => {
            expect(struct.fields[i]!.key).toEqual(key);
            const subtype = struct.fields[i]!.type;
            check_type(subtype, def.struct[key]!);
        })
    }
    else {
        const map = type as quimblos.MapType;
        expect(map).toBeInstanceOf(quimblos.MapType)
        if ('vec' in def) {
            expect(map.kind).toEqual('vec');
            check_type(map.type, def.vec);
        }
        if ('map' in def) {
            expect(map.kind).toEqual('map');
            check_type(map.type, def.map);
        }
        if ('event' in def) {
            expect(map.kind).toEqual('event');
            check_type(map.type, def.event);
        }
    }
}

function check(ast: AST, name: string, def: Exclude<TestTypeDef, string>) {
    const type = check_name(ast, name);
    check_type(type.type, def);
}

describe('[ast] TypeDeclaration', () => {

    // Simple Types
    it('type Vec = [u8]', () => {
        const ast = lib.compile_ast(`
            type Vec = [u8]
        `);
        check(ast, 'Vec', {
            vec: 'u8'
        });
    })
    it('type Map = {u8}', () => {
        const ast = lib.compile_ast(`
            type Map = {u8}
        `);
        check(ast, 'Map', {
            map: 'u8'
        });
    })
    it('type Event = @u8', () => {
        const ast = lib.compile_ast(`
            type Event = @u8
        `);
        check(ast, 'Event', {
            event: 'u8'
        });
    })
    it('type Struct = {a:u8,b:str}', () => {
        const ast = lib.compile_ast(`
            type Struct = {a:u8,b:str}
        `);
        check(ast, 'Struct', {
            struct: {
                'a': 'u8',
                'b': 'str'
            }
        });
    })

    // Complex Types
    it('type VecOfVec = [[u8]]', () => {
        const ast = lib.compile_ast(`
            type VecOfVec = [[u8]]
        `);
        check(ast, 'VecOfVec', {
            vec: {
                vec: 'u8'
            }
        });
    })
    it('type VecOfMap = [{u8}]', () => {
        const ast = lib.compile_ast(`
            type VecOfMap = [{u8}]
        `);
        check(ast, 'VecOfMap', {
            vec: {
                map: 'u8'
            }
        });
    })
    it('type VecOfEvent = [@u8]', () => {
        const ast = lib.compile_ast(`
            type VecOfEvent = [@u8]
        `);
        check(ast, 'VecOfEvent', {
            vec: {
                event: 'u8'
            }
        });
    })
    it('type VecOfStruct = [{a:u8,b:str}]', () => {
        const ast = lib.compile_ast(`
            type VecOfStruct = [{a:u8,b:str}]
        `);
        check(ast, 'VecOfStruct', {
            vec: {
                struct: {
                    'a': 'u8',
                    'b': 'str'
                }
            }
        });
    })
    
    it('type MapOfVec = {[u8]}', () => {
        const ast = lib.compile_ast(`
            type MapOfVec = {[u8]}
        `);
        check(ast, 'MapOfVec', {
            map: {
                vec: 'u8'
            }
        });
    })
    it('type MapOfMap = {{u8}}', () => {
        const ast = lib.compile_ast(`
            type MapOfMap = {{u8}}
        `);
        check(ast, 'MapOfMap', {
            map: {
                map: 'u8'
            }
        });
    })
    it('type MapOfEvent = {@u8}', () => {
        const ast = lib.compile_ast(`
            type MapOfEvent = {@u8}
        `);
        check(ast, 'MapOfEvent', {
            map: {
                event: 'u8'
            }
        });
    })
    it('type MapOfStruct = {{a:u8,b:str}}', () => {
        const ast = lib.compile_ast(`
            type MapOfStruct = {{a:u8,b:str}}
        `);
        check(ast, 'MapOfStruct', {
            map: {
                struct: {
                    'a': 'u8',
                    'b': 'str'
                }
            }
        });
    })
    
    it('type EventOfVec = @[u8]', () => {
        const ast = lib.compile_ast(`
            type EventOfVec = @[u8]
        `);
        check(ast, 'EventOfVec', {
            event: {
                vec: 'u8'
            }
        });
    })
    it('type EventOfMap = @{u8}', () => {
        const ast = lib.compile_ast(`
            type EventOfMap = @{u8}
        `);
        check(ast, 'EventOfMap', {
            event: {
                map: 'u8'
            }
        });
    })
    it('type EventOfEvent = @@u8', () => {
        const ast = lib.compile_ast(`
            type EventOfEvent = @@u8
        `);
        check(ast, 'EventOfEvent', {
            event: {
                event: 'u8'
            }
        });
    })
    it('type EventOfStruct = @{a:u8,b:str}', () => {
        const ast = lib.compile_ast(`
            type EventOfStruct = @{a:u8,b:str}
        `);
        check(ast, 'EventOfStruct', {
            event: {
                struct: {
                    'a': 'u8',
                    'b': 'str'
                }
            }
        });
    })
    
    it('type StructOfVec = {a:u8,b:[u8]}', () => {
        const ast = lib.compile_ast(`
            type StructOfVec = {a:u8,b:[u8]}
        `);
        check(ast, 'StructOfVec', {
            struct: {
                'a': 'u8',
                'b': {
                    vec: 'u8'
                }
            }
        });
    })
    it('type StructOfMap = {a:u8,b:{u8}}', () => {
        const ast = lib.compile_ast(`
            type StructOfMap = {a:u8,b:{u8}}
        `);
        check(ast, 'StructOfMap', {
            struct: {
                'a': 'u8',
                'b': {
                    map: 'u8'
                }
            }
        });
    })
    it('type StructOfEvent = {a:u8,b:@u8}', () => {
        const ast = lib.compile_ast(`
            type StructOfEvent = {a:u8,b:@u8}
        `);
        check(ast, 'StructOfEvent', {
            struct: {
                'a': 'u8',
                'b': {
                    event: 'u8'
                }
            }
        });
    })
    it('type StructOfStruct = {a:u8,b:{c:f32,d:str}}', () => {
        const ast = lib.compile_ast(`
            type StructOfStruct = {a:u8,b:{c:f32,d:str}}
        `);
        check(ast, 'StructOfStruct', {
            struct: {
                'a': 'u8',
                'b': {
                    struct: {
                        'c': 'f32',
                        'd': 'str'
                    }
                }
            }
        });
    })

    // Mixed Types
    it('type Vec = [Map]', () => {
        const ast = lib.compile_ast(`
            type Vec = [Map]
        `);
        check(ast, 'Vec', {
            vec: 'Map'
        });
    })
    it('type Map = {Event}', () => {
        const ast = lib.compile_ast(`
            type Map = {Event}
        `);
        check(ast, 'Map', {
            map: 'Event'
        });
    })
    it('type Event = @Struct', () => {
        const ast = lib.compile_ast(`
            type Event = @Struct
        `);
        check(ast, 'Event', {
            event: 'Struct'
        });
    })
    it('type Struct = {a:Vec,b:Map,c:Event}', () => {
        const ast = lib.compile_ast(`
            type Struct = {a:Vec,b:Map,c:Event}
        `);
        check(ast, 'Struct', {
            struct: {
                'a': 'Vec',
                'b': 'Map',
                'c': 'Event',
            }
        });
    })
})
