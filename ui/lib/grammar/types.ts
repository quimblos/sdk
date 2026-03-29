import { qbscript } from "./semantics"

export const quimblos_types: {
    [type: string]: {
        qb: string,
        allow_array: boolean,
        as_value_type?: string,
        allow_assign_value: qbscript.ValueType[]
        allow_assign_node: qbscript.Type[]
    }
} = {
    // _log_: { qb: '_LOG_', allow_array: false, allow_cast_from: ['err','ptr','bool','uint','int','float','str'] },
    void: { qb: 'VOID',
        allow_array: false,
        as_value_type: undefined,
        allow_assign_value: [],
        allow_assign_node: []
    },
    err: { qb: 'ERROR',
        allow_array: false,
        as_value_type: 'err',
        allow_assign_value: ['String'],
        allow_assign_node: ['err','str']
    },
    ptr: { qb: 'PTR',
        allow_array: false,
        as_value_type: 'ptr',
        allow_assign_value: ['UnsignedInteger'],
        allow_assign_node: ['u8','u16','u32']
    },
    bool: { qb: 'BOOL',
        allow_array: true,
        as_value_type: 'bool',
        allow_assign_value: ['Boolean'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u8: { qb: 'UINT8',
        allow_array: true,
        as_value_type: 'uint',
        allow_assign_value: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i8: { qb: 'INT8',
        allow_array: true,
        as_value_type: 'int',
        allow_assign_value: ['Integer'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u16: { qb: 'UINT16',
        allow_array: true,
        as_value_type: 'uint',
        allow_assign_value: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i16: { qb: 'INT16',
        allow_array: true,
        as_value_type: 'int',
        allow_assign_value: ['Integer'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u32: { qb: 'UINT32',
        allow_array: true,
        as_value_type: 'uint',
        allow_assign_value: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i32: { qb: 'INT32',
        allow_array: true,
        as_value_type: 'int',
        allow_assign_value: ['Integer'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    f32: { qb: 'FLOAT32',
        allow_array: true,
        as_value_type: 'float',
        allow_assign_value: ['Float'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    str: { qb: 'STRING',
        allow_array: true,
        as_value_type: 'str',
        allow_assign_value: ['String'],
        allow_assign_node: ['str','err']
    }
}