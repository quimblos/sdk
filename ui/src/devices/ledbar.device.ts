import './ledbar-device.goo'
import { Device, DeviceDataOut } from "../../lib/kernel";

export class LedBarDevice extends Device<{
    name: string,
    leds: boolean[]
}> {

    private data: DeviceDataOut = {};

    constructor(name = 'LEDBAR') {
        super(name, 'ledbar-device', [
            { name: 'on', bytes: wasm.VectorCode.from([0x10, 0x00]) }
        ]);
    }

    public setup() {
        for (const webc of Object.values(this.webc)) {
            webc.name = this.name;
        }
        this.on_tick(this.data);
    }

    public log(value: DeviceDataOut[string]) {
        return;
    }

    public on_tick(data: DeviceDataOut) {
        this.data = data;
        const value = data.on?.value ?? 0;
        const leds = Array.from({ length: 8 }, (_, i) => Boolean((value >> (7 - i)) & 1));
        if (!this.webc) return;
        for (const webc of Object.values(this.webc)) {
            webc.leds = leds;
            webc.render();
        }
    }
}