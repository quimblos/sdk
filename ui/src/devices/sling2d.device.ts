import './sling2d-device.goo'
import { Device } from "../../lib/kernel";

export class Sling2DDevice extends Device<{
    name: string,
    leds: boolean[]
}> {
    constructor(name = 'SLING2D') {
        super(name, 'sling2d-device', [
            { name: 'x', type: 'u8' },
            { name: 'y', type: 'u8' }
        ]);
    }

    public setup() {
        for (const webc of Object.values(this.webc)) {
            webc.name = this.name;
        }
    }

    public update(regs: any) {
        // const leds = Array.from({ length: 8 }, (_, i) => Boolean((regs[0] >> (7 - i)) & 1));
        // if (!this.webc) return;
        // this.webc.leds = leds;
        // this.webc.render();
    }
}