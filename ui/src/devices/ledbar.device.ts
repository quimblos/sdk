import './ledbar-device.goo'
import { Device } from "../../lib/kernel";

export class LedBarDevice extends Device<{
    name: string,
    leds: boolean[]
}> {
    constructor(name = 'LEDBAR') {
        super(name, 'ledbar-device', ['b8']);
    }

    public setup() {
        this.webc!.name = this.name;
    }

    public update(regs: any) {
        const leds = Array.from({ length: 8 }, (_, i) => Boolean((regs[0] >> (7 - i)) & 1));
        if (!this.webc) return;
        this.webc.leds = leds;
        this.webc.render();
    }
}