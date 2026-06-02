import './sling2d-driver.goo'
import { DriverData, Driver } from "../../lib/driver";

export class Sling2DDriver extends Driver<{
    bytes: number[]
}> {
    constructor(name = 'SLING2D') {
        super(name, 'sling2d-device', [
            { name: 'x', type_def: { use: 0xFC, add: { kind:0, children: new wasm.VectorTypeDef() }, is_const: false } },
            { name: 'y', type_def: { use: 0xFC, add: { kind:0, children: new wasm.VectorTypeDef() }, is_const: false } }
        ]);
    }

    public setup() {
        for (const webc of Object.values(this.webc)) {
            webc.name = this.name;
        }
        this.render({});
    }

    public log(value: any) {
        return;
    }

    public render(data: DriverData) {
        
    }
}