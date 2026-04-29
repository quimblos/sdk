export namespace qasm {

    export const OpCode = {
        USE_DEVICE: 0x01,
        USE_VAR: 0x02,
        SET: 0x10,
        HOLD: 0x1A,
        RELEASE: 0x1B,
        GOTO: 0x20,
        BRANCH: 0x21,
        SET_IF_EQ: 0x30,
        SET_IF_LT: 0x31,
        SET_IF_GT: 0x32,
        NOT: 0x40,
        AND: 0x41,
        OR: 0x42,
        ADD: 0x50,
        SUB: 0x51,
        MULT: 0x52,
        DIV: 0x53,
        MOD: 0x54,
        POW: 0x55,
        SLEEP: 0xE0,
        LOG: 0xEA,
        RETURN: 0xEF,
        RESET: 0xF0,
        REBOOT: 0xFF,
    };
    export type OpCode = typeof OpCode;

    export const Type = {
        void: 0x00,
        null: 0x10,
        err: 0x02,
        bool: 0x0A,
        u8: 0x10,
        i8: 0x11,
        u16: 0x20,
        i16: 0x21,
        u32: 0x40,
        i32: 0x41,
        f32: 0x4A,
        str: 0xA0,
        vec: 0xB0,
        ref: 0xF0
    };
    export type Type = typeof Type;
}