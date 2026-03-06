import './ledstrip-device.goo'
import { Device } from "../../lib/kernel";

export class LedStripDevice extends Device<{
    name: string,
    bytes: number[]
}> {
    constructor(
        name = 'LEDSTRIP',
        private length = 10
    ) {
        super(name, 'ledstrip-device', [
            { type: 'u8', length: 0 }
        ]);
    }

    public setup() {
        this.webc!.name = this.name;
        this.webc!.bytes = Array.from({ length: this.length*3 }).map(() => 0);
    }

    public update(regs: any) {
        // const leds = Array.from({ length: 8 }, (_, i) => Boolean((regs[0] >> (7 - i)) & 1));
        // if (!this.webc) return;
        // this.webc.leds = leds;
        // this.webc.render();
    }
}