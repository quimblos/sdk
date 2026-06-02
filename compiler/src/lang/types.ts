import { quimblos } from "./semantics"

export enum TypeKind {
    VOID = 0x00,    
    BOOL = 0x01,    
    INT = 0x02,     
    FLOAT = 0x03,
    STRING = 0x04,
    REF = 0x05,
    REF_SLICE = 0x06,
    VECTOR = 0x10,  // no built-in types, schema of_map
    MAP = 0x11,     // no built-in types, schema of_map
    STRUCT = 0x12,  // no built-in types, schema of_struct
    EVENT = 0x13,   // no built-in types, schema of_map
    FN = 0x20,      // no built-in types, schema of_map
}

export const builtin_types = {
    'void': {
        kind: TypeKind.VOID
    }
}

export type TypeDef = {
    key?: string
    add: {
        kind: 'vec' | 'map' | 'event' | 'struct',
        children: TypeDef[]
    }
} | {
    key?: string
    use: string
}

export const quimblos_types: {
    [type: string]: {
        qb: string,
        allow_array: boolean,
        as_literal?: string | undefined,
        allow_assign_literal: quimblos.LiteralType[]
        allow_assign_node: quimblos.Type[]
    }
} = {
    // _log_: { qb: '_LOG_', allow_array: false, allow_cast_from: ['err','ptr','bool','uint','Integer','float','str'] },
    void: { qb: 'VOID',
        allow_array: false,
        as_literal: undefined,
        allow_assign_literal: [],
        allow_assign_node: []
    },
    err: { qb: 'ERROR',
        allow_array: false,
        as_literal: undefined,
        allow_assign_literal: ['String'],
        allow_assign_node: ['err','str']
    },
    ptr: { qb: 'PTR',
        allow_array: false,
        as_literal: undefined,
        allow_assign_literal: ['UnsignedInteger'],
        allow_assign_node: ['u8','u16','u32']
    },
    bool: { qb: 'BOOL',
        allow_array: true,
        as_literal: 'Boolean',
        allow_assign_literal: ['Boolean'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u8: { qb: 'UINT8',
        allow_array: true,
        as_literal: 'UnsignedInteger',
        allow_assign_literal: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i8: { qb: 'INT8',
        allow_array: true,
        as_literal: 'Integer',
        allow_assign_literal: ['Integer'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u16: { qb: 'UINT16',
        allow_array: true,
        as_literal: 'UnsignedInteger',
        allow_assign_literal: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i16: { qb: 'INT16',
        allow_array: true,
        as_literal: 'Integer',
        allow_assign_literal: ['Integer'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    u32: { qb: 'UINT32',
        allow_array: true,
        as_literal: 'UnsignedInteger',
        allow_assign_literal: ['Bitmask', 'Hexcode', 'UnsignedInteger'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    i32: { qb: 'INT32',
        allow_array: true,
        as_literal: 'Integer',
        allow_assign_literal: ['Integer'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    f32: { qb: 'FLOAT32',
        allow_array: true,
        as_literal: 'Float',
        allow_assign_literal: ['Float'],
        allow_assign_node: ['bool','u8','i8','u16','i16','u32','i32','f32']
    },
    str: { qb: 'STRING',
        allow_array: true,
        as_literal: 'String',
        allow_assign_literal: ['String'],
        allow_assign_node: ['str','err']
    }
}