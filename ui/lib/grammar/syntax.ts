export const quimblos_syntax = `
/*
    Quimblos
    A high-level language for low-level hardware
*/

grammar ::= (blank? (comment | macro))* (blank? (comment | statement))*



/*
    Comments
*/

comment ::= '//' [#x20-#xD7FF]* eol

/*
    Macros
*/

macro ::= macro_use
macro_use ::= '@use' ws identifier_device eol

statement ::= statement_var | statement_ptr | statement_function | statement_assign | statement_hold | statement_release | statement_if | statement_else_if | statement_else | statement_while | statement_log | statement_sleep | statement_return | statement_reset | statement_reboot

/*
    Declarations
*/

statement_var ::= kw_var ws typed_identifier ws? (op_assign ws? expression)? eol
statement_ptr ::= kw_ptr ws identifier ws? ':' ws? ref_no_idx (ws? op_assign ws? expression)? eol

statement_function ::= 'fn' ws identifier '(' (ws? typed_identifier ',')? ws? typed_identifier? ')' ws? ':' eol

/*
    Statements
*/

statement_assign ::= reference ws? ( op_math? op_assign ws? expression eol )

statement_hold ::= kw_hold ws identifier_device eol
statement_release ::= kw_release ws identifier_device eol

statement_if ::= kw_if ws expression_bool ws? (':' eol)
statement_else_if ::= kw_else ws statement_if
statement_else ::= kw_else ws? ':' eol
statement_while ::= kw_while ws expression_bool ws? (':' eol)

statement_log ::= kw_log ws expression eol
statement_sleep ::= kw_sleep ws unsigned_integer eol
statement_return ::= kw_return (ws expression)? eol
statement_reset ::= kw_reset eol
statement_reboot ::= kw_reboot eol

/*
    Expressions
*/

/*expression ::= expression_bool | expression_math*/
expression ::= (term ws? op ws?)* term

/* expression_bool ::= (bool_term ws? op_bool ws?)* bool_term */
op ::= op_bool|op_math
term ::= ( '(' ws? expression ws? ')' ) | value

/* expression_math ::= (math_term ws? op_math ws?)* math_term */
/* math_term ::= ( '(' ws? expression ws? ')') | value */

/*
    References
*/

reference ::= ref_idx | ref_no_idx
ref_no_idx ::= ref_device | ref_script
ref_idx ::= ref_device_idx | ref_script_idx

ref_script ::= identifier
ref_script_idx ::= identifier ('[' expression ']')
ref_device ::= identifier_device '.' identifier
ref_device_idx ::= identifier_device '.' identifier ('[' expression ']')

/*
    Identifiers
*/

typed_identifier ::= identifier ws? ':' ws? identifier_type ('[' unsigned_integer ']')?

identifier_device ::= letter (letter | digit)*
identifier_type ::= char+
identifier ::= letter (letter | digit)*

/*
    Values

    - true | false
    - #00120343
    - 0b01010101
    - -12.34
    - 1234
    - 'some text'
*/

value ::= primitive | string | reference

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
    Identation
*/

iden ::= ws?

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
kw_while ::= 'while'
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

op_math ::= op_add | op_sub | op_mult | op_div | op_mod | op_pow
op_add ::= '+'
op_sub ::= '-'
op_mult ::= '*'
op_div ::= '/'
op_mod ::= '%'
op_pow ::= '^'

op_bool ::= op_compare | kw_and | kw_or
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