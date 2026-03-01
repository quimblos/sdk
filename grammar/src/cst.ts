/*
    Concrete Syntax Tree

    This tree represents the code as parsed by the `ebnf` lib.
    The semantics parser is responsible for transforming this
    tree into an AST (Abstract Syntax Tree).
*/

export type CSTNodeType = 
    'grammar'
    | 'statement'
    | 'comment'
    | 'directive'
    | 'directive_device'
    | 'flow_statement'
    | 'if_statement'
    | 'if_else_statement'
    | 'loop_statement'
    | 'while_statement'
    | 'break_statement'
    | 'continue_statement'
    | 'assignment'
    | 'declaration'
    | 'runner_statement'
    | 'sleep_statement'
    | 'log_statement'
    | 'stop_statement'
    | 'error_statement'
    | 'art'
    | 'comparison'
    | 'variable'
    | 'word'
    | 'primitive'
    | 'boolean'
    | 'hexcode'
    | 'float'
    | 'int'
    | 'uint'
    | 'string'
    | 'char'
    | 'kw_true'
    | 'kw_false'
    | 'kw_device'
    | 'kw_sleep'
    | 'kw_log'
    | 'kw_stop'
    | 'kw_error'
    | 'kw_if'
    | 'kw_else'
    | 'kw_end'
    | 'kw_break'
    | 'kw_continue'
    | 'kw_loop'
    | 'kw_type'
    | 'op_assign'
    | 'op_art'
    | 'op_sum'
    | 'op_sub'
    | 'op_mult'
    | 'op_div'
    | 'op_mod'
    | 'op_compare'
    | 'op_eq'
    | 'op_neq'
    | 'op_gt'
    | 'op_lt'
    | 'op_gteq'
    | 'op_lteq'
    | 'letter'
    | 'digit'
    | 'special_char'
    | 'blank'
    | 'eol'
    | 'ws'

export type CSTNode = {
    type: CSTNodeType,
    children: CSTNode[],
    start: number,
    end: number,
    text: string,
    errors: { message: string }[],
}