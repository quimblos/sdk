import { Style } from "../lang-maker/style";

export const quimblos_style: Style = {
    suffix: 'syntax_error',
    syntax: {
        'comment': 'comment',
        'macro_use': 'macro',

        'identifier_device': 'address',
        'identifier_type': 'type',
        'identifier': 'identifier',

        'kw_var': 'keyword',
        'kw_ptr': 'keyword',
        'kw_hold': 'keyword',
        'kw_release': 'keyword',
        'kw_goto': 'keyword',
        'kw_if': 'keyword',
        'kw_else': 'keyword',
        'kw_while': 'keyword',
        'kw_and': 'keyword',
        'kw_or': 'keyword',
        'kw_break': 'keyword',
        'kw_continue': 'keyword',
        'kw_log': 'keyword',
        'kw_sleep': 'keyword',
        'kw_return': 'keyword',
        'kw_reset': 'keyword',
        'kw_reboot': 'keyword',

        'op_assign': 'operator',
        'op_math': 'operator',
        
        'boolean': 'primitive',
        'hexcode': 'primitive',
        'bitmask': 'primitive',
        'float': 'primitive',
        'integer': 'primitive',
        'unsigned_integer': 'primitive',
        'string': 'string'
    },
    css: {
        'ast_error': {
            'background-color':'#bb333388'
        },
        'comment': {
            color:'#666'
        },
        'iden': {
            color:'#222'
        },
        'syntax_error': {
            'background-color':'#bb333322'
        },
        'macro': {
            color: '#b08'
        },
        'keyword': {
            color: '#08b'
        },
        'type': {
            color: '#8b0'
        },
        'operator': {
            color: '#0b8'
        },
        'address': {
            color: '#b80'
        },
        'identifier': {
            color: '#b8b'
        },
        'primitive': {
            color: '#8bb'
        },
        'string': {
            color: '#bb8'
        },
    }
}