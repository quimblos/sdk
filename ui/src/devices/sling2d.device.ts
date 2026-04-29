import './sling2d-device.goo'
import { Device, DeviceDataOut } from "../../lib/kernel";

export class Sling2DDevice extends Device<{
    name: string,
    leds: boolean[]
}> {
    constructor(name = 'SLING2D') {
        super(name, 'sling2d-device', [
            { name: 'x', bytes: wasm.VectorCode.from([0x10, 0x00]) },
            { name: 'y', bytes: wasm.VectorCode.from([0x10, 0x00]) }
        ]);
    }

    public setup() {
        for (const webc of Object.values(this.webc)) {
            webc.name = this.name;
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