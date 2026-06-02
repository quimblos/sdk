import './led8-driver.goo'
import { DriverData, Driver } from "../../lib/driver";

export class LED8Driver extends Driver<{
    leds: boolean[]
}> {

    constructor(name = 'LED8') {
        super(name, 'led8-device', [
            { name: 'on', type_def: { use: 0xFC, add: { kind:0, children: new wasm.VectorTypeDef() }, is_const: false } }
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
        // this.data = data;
        const value = data[0].value ?? 0;
        const leds = Array.from({ length: 8 }, (_, i) => Boolean((value >> (7 - i)) & 1));
        if (!this.webc) return;
        for (const webc of Object.values(this.webc)) {
            webc.leds = leds;
            webc.render();
        }
    }
}