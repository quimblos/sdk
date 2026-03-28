import { qbscript } from "./semantics"

export const quimblos_types: {
    [type: string]: {
        qb: string,
        allow_array: boolean,
        as_value_type?: string,
        allow_assign_value: qbscript.ValueType[]
        allow_cast_from: qbscript.Type[]
    }
} = {
    // _log_: { qb: '_LOG_', allow_array: false, allow_cast_from: ['err','ptr','bool','uint','int','float','str'] },
    void: { qb: 'VOID',
        allow_array: false,
        as_value_type: undefined,
        allow_assign_value: [],
        allow_cast_from: []
    },
    err: { qb: 'ERROR',
        allow_array: false,
        as_value_type: 'err',
        allow_assign_value: ['String'],
        allow_cast_from: ['err','str']
    },
    ptr: { qb: 'PTR',
        allow_array: false,
        as_value_type: 'ptr',
        allow_assign_value: ['UnsignedInteger'],
        allow_cast_from: ['u8','u16','u32']
    },
    bool: { qb: 'BOOL',
        allow_array: true,
        as_value_type: 'bool',
        allow_assign_value: ['Boolean'],
        allow_cast_from: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u8: { qb: 'UINT8',
        allow_array: true,
        as_value_type: 'uint',
        allow_assign_value: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_cast_from: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i8: { qb: 'INT8',
        allow_array: true,
        as_value_type: 'int',
        allow_assign_value: ['Integer'],
        allow_cast_from: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u16: { qb: 'UINT16',
        allow_array: true,
        as_value_type: 'uint',
        allow_assign_value: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_cast_from: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i16: { qb: 'INT16',
        allow_array: true,
        as_value_type: 'int',
        allow_assign_value: ['Integer'],
        allow_cast_from: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u32: { qb: 'UINT32',
        allow_array: true,
        as_value_type: 'uint',
        allow_assign_value: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_cast_from: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i32: { qb: 'INT32',
        allow_array: true,
        as_value_type: 'int',
        allow_assign_value: ['Integer'],
        allow_cast_from: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    f32: { qb: 'FLOAT32',
        allow_array: true,
        as_value_type: 'float',
        allow_assign_value: ['Float'],
        allow_cast_from: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    str: { qb: 'STRING',
        allow_array: true,
        as_value_type: 'str',
        allow_assign_value: ['String'],
        allow_cast_from: ['str','err']
    },
    arr: { qb: 'ARRAY',
        allow_array: false,
        as_value_type: undefined,
        allow_assign_value: [],
        allow_cast_from: []
    },
}