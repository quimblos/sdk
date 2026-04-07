export namespace qasm {

    export const OpBind = {
        NODE_NODE: 0x00,
        NODE_REF: 0x01,
        REF_NODE: 0x02,
        REF_REF: 0x03,
        SHORT_NODE_NODE: 0x10,
        SHORT_NODE_REF: 0x11,
        SHORT_REF_NODE: 0x12,
        SHORT_REF_REF: 0x13
    };
    export type OpBind = typeof OpBind;

    export const OpCode = {
        USE_DEVICE: 0x01,
        USE_NODE: 0x02,
        USE_NODE_ALIASED: 0x03,
        SET: 0x10,
        HOLD: 0x1A,
        RELEASE: 0x1B,
        GOTO: 0x20,
        IF_EQ: 0x21,
        IF_LT: 0x25,
        IF_GT: 0x23,
        ADD: 0x30,
        SUB: 0x31,
        MULT: 0x32,
        DIV: 0x33,
        MOD: 0x34,
        POW: 0x35,
        LOG: 0xD0,
        SLEEP: 0xE0,
        RETURN: 0xEE,
        RESET: 0xF0,
        REBOOT: 0xFF,
    };
    export type OpCode = typeof OpCode;

    export const Type = {
        void: 0x00,
        err: 0xFF,
        ptr: 0x02,
        ptr_short: 0x01,
        bool: 0x0A,
        u8: 0x10,
        i8: 0x11,
        u16: 0x20,
        i16: 0x21,
        u32: 0x40,
        i32: 0x41,
        f32: 0x4A,
        str: 0xF0,
        str_short: 0xF0,
        arr: 0xE0,
        arr_short: 0xE1,
    };
    export type Type = typeof Type;
}