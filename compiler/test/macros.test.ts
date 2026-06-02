import * as lib from "./lib";

describe('UseDriverMacro', () => {
    it.only('#driver NAME', () => {
        const compiled = lib.compile(`
            #driver LED
        `);
        expect(compiled.macros).toEqual([
            { op: 'USE_DRIVER', name: 'LED' }
        ])
    })
})

describe('UseDriverMacro', () => {
    it('#topic NAME', () => {
        const compiled = lib.compile(`
            #topic log
        `);
        expect(compiled.macros).toEqual([
            { op: 'USE_TOPIC', name: 'log' }
        ])
    })
})
