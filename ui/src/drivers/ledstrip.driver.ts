import './ledstrip-driver.goo'
import { DriverData, Driver } from "../../lib/driver";

export class LedStripDriver extends Driver<{
    bytes: number[]
}> {
    constructor(name = 'LEDSTRIP') {
        super(name, 'ledstrip-device', [
            { name: 'data', type_def: { use: 0xFC, add: { kind:0, children: new wasm.VectorTypeDef() }, is_const: false } }
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