type __qb_Value = number | boolean | string

enum __qb_Type {
    UINT8,
    INT8,
    UINT16,
    INT16,
    FLOAT
}

abstract class __qb_Register {
    constructor(
        public name: string,
        public type: __qb_Type,
        public value: __qb_Value
    ) {}
}

abstract class __qb_Device {

    private registers: __qb_Register[];

    constructor(
        public name: string,
        registers: {
            name: string
            type: __qb_Type
        }[]
    ) {}

    public get(reg_i: number): __qb_Value {
        return this.registers[reg_i]?.value;
    };
    public set(reg_i: number, value: __qb_Value) {
        if (!this.registers[reg_i]) return;
        this.registers[reg_i].value = value;
    };
}

export class WaterTankDevice extends __qb_Device {

    constructor() {
        super(
            'tank',
            [
                { name: 'VALVE', type: __qb_Type.UINT8 },
                { name: 'SENSOR_VOLUME', type: __qb_Type.FLOAT },
            ]
        )
        console.log('Built the water tank!');
    }

}