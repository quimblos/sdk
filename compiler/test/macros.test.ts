import * as lib from "./lib";

describe('UseDeviceMacro', () => {
    it('#use macro', () => {
        const compiled = lib.compile(`
            #use LEDBAR
        `);
        expect(compiled.code).toEqual([
            { op: 'USE_DEVICE', name: 'LEDBAR' }
        ])
    })
})
