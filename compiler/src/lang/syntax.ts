export const quimblos_syntax = `
/*
    Quimblos EBNF syntax
*/

grammar ::= (blank? (comment | macro))* (blank? (comment | statement))*

/*
    Comments
*/

comment ::= '//' [#x20-#xD7FF]* eol

/*
    Macros
*/

macro ::= macro_use_driver | macro_use_topic
macro_use_driver ::= '#driver' ws identifier eol
macro_use_topic ::= '#topic' ws identifier eol

statement ::= declaration_type | declaration_var | declaration_function | statement_assign | statement_hold | statement_release | statement_if | statement_else_if | statement_else | statement_while | statement_log | statement_sleep | statement_return | statement_reboot

/*
    Declarations
*/

declaration_type ::= kw_type ws identifier ws? op_assign ws? type eol

declaration_var ::= (kw_const|kw_var) ws (typed_identifier|identifier) ws? (op_assign ws? expression)? eol

declaration_function ::= 'fn' ws identifier '(' (ws? typed_identifier ',')? ws? typed_identifier? ')' ws? ':' eol

/*
    Statements
*/

statement_assign ::= reference ws? ( op_math? op_assign ws? expression eol )

statement_hold ::= kw_hold ws identifier eol
statement_release ::= kw_release ws identifier eol

statement_if ::= kw_if ws expression ws? (':' eol)
statement_else_if ::= kw_else ws statement_if
statement_else ::= kw_else ws? ':' eol
statement_while ::= kw_while ws expression ws? (':' eol)

statement_log ::= kw_log ws expression eol
statement_sleep ::= kw_sleep ws expression eol
statement_return ::= kw_return (ws expression)? eol
statement_reboot ::= kw_reboot eol

/*
    Expressions
*/

expression ::= (term ws? op ws?)* term

op ::= op_bool|op_math
term ::= ( '(' ws? expression ws? ')' ) | value

/*
    References
*/

reference ::= ref_driver | ref_script

ref_script ::= identifier ('[' expression ']')+
ref_driver ::= identifier '.' identifier ('[' expression ']')+

/*
    Identifiers
*/

typed_identifier ::= identifier ws? ':' ws? type ('[' unsigned_integer ']')?

identifier ::= letter char*

/*
    Types
*/

type ::= identifier | type_vec | type_event | type_struct | type_map
type_vec ::= ('[' blank? type blank? ']')
type_event ::= ('@' type)
type_struct ::= '{' blank? type_struct_field (ws? ',' blank? type_struct_field)* blank? '}'
type_struct_field ::= identifier ws? ':' ws? type
type_map ::= ('{' blank? type blank? '}')

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

kw_type ::= 'type'
kw_const ::= 'const'
kw_var ::= 'var'

kw_hold ::= 'hold'
kw_release ::= 'release'

kw_goto ::= 'goto'
kw_if ::= 'if'
kw_else ::= 'else'
kw_while ::= 'while'
kw_and ::= 'and'
kw_or ::= 'or'
kw_xor ::= 'xor'
kw_break ::= 'break'
kw_continue ::= 'continue'

kw_log ::= 'log'
kw_sleep ::= 'sleep'
kw_return ::= 'return'
kw_reboot ::= 'reboot'

op_assign ::= '='

op_math ::= op_add | op_sub | op_mult | op_div | op_mod | op_pow
op_add ::= '+'
op_sub ::= '-'
op_mult ::= '*'
op_div ::= '/'
op_mod ::= '%'
op_pow ::= '^'

op_bool ::= op_compare | kw_and | kw_or | kw_xor
op_compare ::= op_gteq | op_lteq | op_neq | op_eq | op_gt | op_lt
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