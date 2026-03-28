export const quimblos_syntax = `
/*
    Quimblos
    A high-level language for low-level hardware
*/

grammar ::= blank? ((comment | macro | declaration | statement) blank?)+

/*
    Comments
*/

comment ::= '//' [#x20-#xD7FF]* eol

/*
    Macros
*/

macro ::= macro_use
macro_use ::= '@use' ws identifier eol

/*
    Declarations
*/

declaration ::= var_declaration | ptr_declaration | function_declaration

var_declaration ::= kw_var ws typed_identifier (ws? op_assign ws? value)? eol
ptr_declaration ::= kw_ptr ws identifier ws? ':' ws? port_identifier (ws? op_assign ws? unsigned_integer)? eol

function_declaration ::= 'fn' ws identifier '(' (ws? typed_identifier ',')? ws? typed_identifier? ')' ws? ':' eol

/*
    Statements
*/

statement ::= statement_assignment | statement_hold | statement_release | statement_if | statement_else_if | statement_else | statement_log | statement_sleep | statement_return | statement_reset | statement_reboot

statement_assignment ::= node ws? op_assign ws? (expression_math | value) eol

statement_hold ::= kw_hold ws identifier eol
statement_release ::= kw_release ws identifier eol

statement_if ::= kw_if ws bool_statement ws? ':' eol
statement_else_if ::= kw_else ws statement_if
statement_else ::= kw_else ws? ':' eol

statement_log ::= kw_log ws (node | value) eol
statement_sleep ::= kw_sleep ws unsigned_integer eol
statement_return ::= kw_return ws (node | value) eol
statement_reset ::= kw_reset eol
statement_reboot ::= kw_reboot eol

/*
    Arithmetics
*/

expression_bool ::= (bool_group ws (kw_and | kw_or) ws)* bool_group
bool_group ::= (bool_term | ( '(' ws? expression_bool ws? ')'))
bool_term ::= (node | value) ws? op_compare ws? (node | value)

expression_math ::= (math_term ws? op_art ws?)* math_term
math_term ::= ((node | value) | ( '(' ws? expression_math ws? ')'))

/*
    Identifiers
*/

node ::= identifier ('.' identifier)? ('[' (unsigned_integer | identifier) ']')?
port_identifier ::= identifier '.' identifier
typed_identifier ::= identifier ws? ':' ws? type_identifier
identifier ::= letter (letter | digit)*

type_identifier ::= char+

/*
    Values

    - true | false
    - #00120343
    - 0b01010101
    - -12.34
    - 1234
    - 'some text'
*/

value ::= primitive | string
primitive ::= boolean | hexcode | bitmask | float | unsigned_integer | integer

boolean ::= 'true' | 'false'
hexcode ::= '#' [a-fA-F0-9]+
bitmask ::= '0b' [01]+
float ::= '-'? ws? ((digit+ '.' digit+)|(digit+ '.')|('.' digit+))
integer ::= '-'? ws? digit+
unsigned_integer ::= digit+

string ::= '\\'' (char | ws)* '\\''
char ::= letter | digit | special_char

/*
    Keywords
*/

kw_var ::= 'var'
kw_ptr ::= 'ptr'

kw_hold ::= 'hold'
kw_release ::= 'release'

kw_goto ::= 'goto'
kw_if ::= 'if'
kw_else ::= 'else'
kw_and ::= 'and'
kw_or ::= 'or'
kw_break ::= 'break'
kw_continue ::= 'continue'

kw_log ::= 'log'
kw_sleep ::= 'sleep'
kw_return ::= 'return'
kw_reset ::= 'reset'
kw_reboot ::= 'reboot'

op_assign ::= '='

op_art ::= op_add | op_sub | op_mult | op_div | op_mod | op_pow
op_add ::= '+'
op_sub ::= '-'
op_mult ::= '*'
op_div ::= '/'
op_mod ::= '%'
op_pow ::= '^'

op_compare ::= op_eq | op_neq | op_gt | op_lt | op_gteq | op_lteq
op_eq ::= '=='
op_neq ::= '!='
op_gt ::= '>'
op_lt ::= '<'
op_gteq ::= '>='
op_lteq ::= '<='

/*
    Lexical
*/

letter ::= [a-zA-Z] | [#xC0-#xFF] /* Latin-1 */
digit ::= [0-9]
special_char ::= [-._/]

blank ::= [#xA0#x20#x09#x0A#x0D]+ /* &nbsp; | Space | Tab | \n | \r */
eol ::= ws* [#x0A#x0D#x00] /* \n | \r | \0 */
ws ::= [#xA0#x20#x09]+ /* &nbsp; | Space | Tab */
`