import { Kernel } from '../src/kernel';
import { QuimblosCompiler } from "../src/compiler"
import { quimblos_syntax } from "../src/lang/syntax";
import { quimblos_semantics } from "../src/lang/semantics";
import { AST, make_syntax_parser } from "@quimblos/langmaker";
import { make_quimblos_linter } from "../src/lang/linter";

const kernel: Kernel = {
    get_device(name: string) {
        return {
            name: 'LEDBAR',
            nodes: [{ name: 'on'}]
        }
    }
}

const compile = (input: string) => {
    const syntax_parser = make_syntax_parser(quimblos_syntax);
    const ast = new AST(quimblos_semantics);
    const linter = make_quimblos_linter(kernel);

    const cst = syntax_parser(input);
    ast.update(cst);
    linter.lint(ast);

    const compiled = QuimblosCompiler.compile(kernel, ast);
    if (ast.errors.length) {
        throw new Error(`AST contains errors: ${ast.errors.map(e => e.error).join(',')}`)
    }
    return compiled;
}

describe('Compiler', () => {

    describe('Macros', () => {
        describe('UseDeviceMacro', () => {
            it('#use macro', () => {
                const compiled = compile(`
                    #use LEDBAR
                `);
                expect(compiled.code).toEqual([
                    { op: 'USE_DEVICE', name: 'LEDBAR' }
                ])
            })
        })
    })
    
    describe ('Declarations', () => {
        describe('VariableDeclaration', () => {
            it('var abc:u8', () => {
                const compiled = compile(`
                    var abc:u8
                `);
                expect(compiled.nodes).toEqual({
                    'abc': { name: 'abc', type: { name: 'u8' }}
                })
            })
            
            it('var abc:f32', () => {
                const compiled = compile(`
                    var abc:f32
                `);
                expect(compiled.nodes).toEqual({
                    'abc': { name: 'abc', type: { name: 'f32' }}
                })
            })
    
            it('var abc:str', () => {
                const compiled = compile(`
                    var abc:str
                `);
                expect(compiled.nodes).toEqual({
                    'abc': { name: 'abc', type: { name: 'str' }}
                })
            })
    
            it('var abc:u8[3]', () => {
                const compiled = compile(`
                    var abc:u8[3]
                `);
                expect(compiled.nodes).toEqual({
                    'abc': { name: 'abc', type: { name: 'arr', item: 'u8', length: 3 }}
                })
            })
    
            it('var abc:f32[3]', () => {
                const compiled = compile(`
                    var abc:f32[3]
                `);
                expect(compiled.nodes).toEqual({
                    'abc': { name: 'abc', type: { name: 'arr', item: 'f32', length: 3 }}
                })
            })
    
            it('var abc:str[3]', () => {
                const compiled = compile(`
                    var abc:str[3]
                `);
                expect(compiled.nodes).toEqual({
                    'abc': { name: 'abc', type: { name: 'arr', item: 'str', length: 3 }}
                })
            })
        })
        describe('PointerDeclaration', () => {
            it('ptr y:x', () => {
                const compiled = compile(`
                    var x:u8\n
                    ptr y:x
                `);
                expect(compiled.nodes).toEqual({
                    'x': { name: 'x', type: { name: 'u8' }},
                    'y': { name: 'y', type: { name: 'ptr' }}
                })
            })
    
            it('ptr y:LEDBAR.on', () => {
                const compiled = compile(`
                    #use LEDBAR\n
                    ptr y:LEDBAR.on
                `);
                expect(compiled.nodes).toEqual({
                    'y': { name: 'y', type: { name: 'ptr' }}
                })
            })
        })
    })

    describe('Statements', () => {
        describe('AssignStatement', () => {
            it('x = 1', () => {
                const compiled = compile(`
                    var x:u8\n
                    x = 1
                `);
                expect(compiled.code).toEqual([
                    { op: 'SET', bind: 'NODE_NODE', target: { device: undefined, node: 'x' }, source: { type: { name: 'u32' }, value: 1 } }
                ])
            })
            it('x = \'test\'', () => {
                const compiled = compile(`
                    var x:str\n
                    x = 'test'
                `);
                expect(compiled.code).toEqual([
                    { op: 'SET', bind: 'NODE_NODE', target: { device: undefined, node: 'x' }, source: { type: { name: 'str' }, value: 'test' } }
                ])
            })
        })
        describe('SleepStatement', () => {
            it('sleep 0', () => {
                const compiled = compile(`
                    sleep 0
                `);
                expect(compiled.code).toEqual([
                    { op: 'SLEEP', time: 0 }
                ])
            })
            it('sleep 1000', () => {
                const compiled = compile(`
                    sleep 1000
                `);
                expect(compiled.code).toEqual([
                    { op: 'SLEEP', time: 1000 }
                ])
            })
        })
        describe('LogStatement', () => {
            it('log true', () => {
                const compiled = compile(`
                    log true
                `);
                expect(compiled.code).toEqual([
                    { op: 'LOG', device: '_$_', source: { type: { name: 'bool' }, value: true } }
                ])
            })
            it('log 0', () => {
                const compiled = compile(`
                    log 0
                `);
                expect(compiled.code).toEqual([
                    { op: 'LOG', device: '_$_', source: { type: { name: 'u32' }, value: 0 } }
                ])
            })
            it('log \'test\'', () => {
                const compiled = compile(`
                    log 'test'
                `);
                expect(compiled.code).toEqual([
                    { op: 'LOG', device: '_$_', source: { type: { name: 'str' }, value: 'test' } }
                ])
            })
            // it('log x', () => {
            //     const compiled = compile(`
            //         var x:u8\n
            //         log x
            //     `);
            //     expect(compiled.code).toEqual([
            //         { op: 'LOG', device: '_$_', type: { name: 'u8' }, value: { node: 'x' } }
            //     ])
            // })
            
        })
    })

    describe('Expressions', () => {
        describe('Math', () => {
            it('1', () => {
                const compiled = compile(`
                    log 1
                `);
                expect(compiled.code).toEqual([
                    { op: 'LOG', device: '_$_', source: { type: { name: 'u32' }, value: 1 } }
                ])
            })
            it('1+2', () => {
                const compiled = compile(`
                    log 1+2
                `);
                expect(compiled.code).toEqual([
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 1 } },
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 2 } },
                    { op: 'LOG', device: '_$_', source: { type: { name: 'ptr' }, value: { node: '_v_0' } } }
                ])
            })
            it('2-1', () => {
                const compiled = compile(`
                    log 2-1
                `);
                expect(compiled.code).toEqual([
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 2 } },
                    { op: 'SUB', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 1 } },
                    { op: 'LOG', device: '_$_', source: { type: { name: 'ptr' }, value: { node: '_v_0' } } }
                ])
            })
            it('1+2-3', () => {
                const compiled = compile(`
                    log 1+2-3
                `);
                expect(compiled.code).toEqual([
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 1 } },
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 2 } },
                    { op: 'SUB', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 3 } },
                    { op: 'LOG', device: '_$_', source: { type: { name: 'ptr' }, value: { node: '_v_0' } } }
                ])
            })
            it('-1-2+3+4', () => {
                const compiled = compile(`
                    log -1-2+3+4
                `);
                expect(compiled.code).toEqual([
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'i32' }, value: -1 } },
                    { op: 'SUB', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 2 } },
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 3 } },
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 4 } },
                    { op: 'LOG', device: '_$_', source: { type: { name: 'ptr' }, value: { node: '_v_0' } } }
                ])
            })
            it('1*2', () => {
                const compiled = compile(`
                    log 1*2
                `);
                expect(compiled.code).toEqual([
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 1 } },
                    { op: 'MULT', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 2 } },
                    { op: 'LOG', device: '_$_', source: { type: { name: 'ptr' }, value: { node: '_v_0' } } }
                ])
            })
            it('1*2/3', () => {
                const compiled = compile(`
                    log 1*2/3
                `);
                expect(compiled.code).toEqual([
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 1 } },
                    { op: 'MULT', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 2 } },
                    { op: 'DIV', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 3 } },
                    { op: 'LOG', device: '_$_', source: { type: { name: 'ptr' }, value: { node: '_v_0' } } }
                ])
            })
            it('1*2+3*4', () => {
                const compiled = compile(`
                    log 1*2+3*4
                `);
                console.log(JSON.stringify(compiled.code, undefined, 2));
                expect(compiled.code).toEqual([
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 1 } },
                    { op: 'MULT', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 2 } },
                    { op: 'ADD', target: { node: '_v_1' }, source: { type: { name: 'u32' }, value: 3 } },
                    { op: 'MULT', target: { node: '_v_1' }, source: { type: { name: 'u32' }, value: 4 } },
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'ptr' }, value: { node: '_v_1'} } },
                    { op: 'LOG', device: '_$_', source: { type: { name: 'ptr' }, value: { node: '_v_0' } } }
                ])
            })
            it.only('1+2-3*4/5+6-7*8/9', () => {
                const compiled = compile(`
                    log 1+2-3*4/5+6-7*8/9
                    log 0 1 2 3 4 5 6 7 8
                `);
                // console.log(JSON.stringify(compiled.code, undefined, 2));
                expect(compiled.code).toEqual([
                    { op: 'ADD', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 3 } },
                    { op: 'MULT', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 4 } },
                    { op: 'DIV', target: { node: '_v_0' }, source: { type: { name: 'u32' }, value: 5 } },
                    { op: 'ADD', target: { node: '_v_1' }, source: { type: { name: 'u32' }, value: 7 } },
                    { op: 'MULT', target: { node: '_v_1' }, source: { type: { name: 'u32' }, value: 8 } },
                    { op: 'DIV', target: { node: '_v_1' }, source: { type: { name: 'u32' }, value: 9 } },
                    { op: 'ADD', target: { node: '_v_2' }, source: { type: { name: 'u32' }, value: 1 } },
                    { op: 'ADD', target: { node: '_v_2' }, source: { type: { name: 'u32' }, value: 2 } },
                    { op: 'SUB', target: { node: '_v_2' }, source: { type: { name: 'ptr' }, value: { node: '_v_0' } } },
                    { op: 'ADD', target: { node: '_v_2' }, source: { type: { name: 'u32' }, value: 6 } },
                    { op: 'SUB', target: { node: '_v_2' }, source: { type: { name: 'ptr' }, value: { node: '_v_1' } } },
                    { op: 'LOG', device: '_$_', source: { type: { name: 'ptr' }, value: { node: '_v_0' } } }
                ])
            })
        })
    })


})