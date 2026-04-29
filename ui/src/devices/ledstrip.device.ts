import './ledstrip-device.goo'
import { Device, DeviceDataOut } from "../../lib/kernel";

export class LedStripDevice extends Device<{
    name: string,
    bytes: number[]
}> {
    constructor(
        name = 'LEDSTRIP',
        private length = 10
    ) {
        super(name, 'ledstrip-device', [
            { name: 'data', bytes: wasm.VectorCode.from([0x10, 0x00]) }
        ]);
    }

    public setup() {
        for (const webc of Object.values(this.webc)) {
            webc.name = this.name;
            webc.bytes = Array.from({ length: this.length*3 }).map(() => 0);
        }
    }
    
    public log(value: DeviceDataOut[string]) {
        return;
    }

    public on_tick(data: DeviceDataOut) {
        // const leds = Array.from({ length: 8 }, (_, i) => Boolean((regs[0] >> (7 - i)) & 1));
        // if (!this.webc) return;
        // this.webc.leds = leds;
        // this.webc.render();
    }
}