import { Style } from "../lang-maker/styling";

export const quimblos_style: Style = {
    suffix: 'syntax_error',
    syntax: {
        'comment': 'comment',
        'port_identifier': 'address',
        'type_identifier': 'type',
        'kw_var': 'keyword',
        'kw_ptr': 'keyword',
        'macro_use': 'macro',
        'op_assign': 'operator',
        'primitive': 'primitive',
        'string': 'string'
    },
    css: {
        'ast_error': {
            'background-color':'#bb333388'
        },
        'comment': {
            color:'#666'
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
        'primitive': {
            color: '#8bb'
        },
        'string': {
            color: '#bb8'
        },
    }
}