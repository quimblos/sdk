import './ledbar-device.goo'
import { Device } from "../../lib/kernel";

export class LedBarDevice extends Device<{
    name: string,
    leds: boolean[]
}> {

    private regs: any[] = [];

    constructor(name = 'LEDBAR') {
        super(name, 'ledbar-device', [
            { type: 'u8', length: 0 }
        ]);
    }

    public setup() {
        for (const webc of Object.values(this.webc)) {
            webc.name = this.name;
        }
        this.update(this.regs);
    }

    public update(regs: any[]) {
        this.regs = regs;
        const leds = Array.from({ length: 8 }, (_, i) => Boolean((regs[0] >> (7 - i)) & 1));
        if (!this.webc) return;
        for (const webc of Object.values(this.webc)) {
            webc.leds = leds;
            webc.render();
        }
    }
}