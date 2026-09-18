#include "semantics/parser.h"

#include "syntax/impl.h"
const CSTNode* parse_eol(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_letter(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_word(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_ws(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_value(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_value_0(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_value_0_2(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_prop(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_rule(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_declaration(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_declaration_1(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_grammar(const std::string& input, uint32_t n, uint32_t i, uint8_t term);

_STX_RULE(eol,RULE,
  if (ti == 0) _STX_TERM_RULE(ws,0,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_OPTIONAL, "eol", "ws"),
    _STX__AFTER_ZERO_OR_N,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 1) _STX_TERM_CHARMAP(1,
    ch == 10 || ch == 13 || ch == 0,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("[\\10\\13\\0]", "eol")
  )
  else break;
,
 if (ti <= 1) _STX__ERROR_REQUIRED("[\\10\\13\\0]")
)

_STX_RULE(letter,RULE,
  if (ti == 0) _STX_TERM_CHARMAP(0,
    (ch >= 97 && ch <= 122) || (ch >= 65 && ch <= 90),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("[\\97-\\122\\65-\\90]", "letter")
  )
  else break;
,
 if (ti <= 0) _STX__ERROR_REQUIRED("[\\97-\\122\\65-\\90]")
)

_STX_RULE(word,RULE,
  if (ti == 0) _STX_TERM_RULE(letter,0,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_MANY(0, "letter", false), "word", "letter"),
    _STX__AFTER_ONE_OR_N,
    _STX__ELSE_MANY(0)
    _STX__ELSE_REQUIRED_STOP("letter", "word")
  )
  else break;
,
 if (ti <= 0) _STX__ERROR_REQUIRED("letter")
)

_STX_RULE(ws,RULE,
  if (ti == 0) _STX_TERM_CHARMAP(0,
    ch == 160 || ch == 32 || ch == 9,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("[\\160\\32\\9]", "ws")
  )
  else break;
,
 if (ti <= 0) _STX__ERROR_REQUIRED("[\\160\\32\\9]")
)

_STX_RULE(value,RULE,
  if (ti == 0) _STX_TERM_RULE(value_0,0,
_STX__ON_ERROR_OR,
_STX__AFTER_OR,
_STX__ELSE_OR  )
  if (ti == 2) _STX_TERM_LITERAL(2, "#", 1,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("'#'", "value")
  )
  else break;
,
if (i == start) _STX__ERROR_REQUIRED("value")
)

_STX_RULE(value_0,GROUP,
  if (ti == 0) _STX_TERM_LITERAL(0, "#", 1,
    _STX__AFTER_OPTIONAL,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 1) _STX_TERM_RULE(word,1,
    _STX__ON_ERROR_NONFAIL(GROUP,_STX__ON_ERROR_MOD_REQUIRED, "value_0", "word"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("word", "value_0")
  )
  if (ti == 2) _STX_TERM_RULE(value_0_2,2,
    _STX__ON_ERROR_NONFAIL(GROUP,_STX__ON_ERROR_MOD_OPTIONAL, "value_0", "Group"),
    _STX__AFTER_OPTIONAL,
    _STX__ELSE_OPTIONAL()
  )
  else break;
,
 if (ti <= 1) _STX__ERROR_REQUIRED("word")
)

_STX_RULE(value_0_2,GROUP,
  if (ti == 0) _STX_TERM_LITERAL(0, "[", 1,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("'['", "value_0_2")
  )
  if (ti == 1) _STX_TERM_LITERAL(1, "]", 1,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("']'", "value_0_2")
  )
  else break;
,
 if (ti <= 0) _STX__ERROR_REQUIRED("'['")
 else if (ti <= 1) _STX__ERROR_REQUIRED("']'")
)

_STX_RULE(prop,RULE,
  if (ti == 0) _STX_TERM_RULE(word,0,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_REQUIRED, "prop", "word"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("word", "prop")
  )
  if (ti == 1) _STX_TERM_RULE(ws,1,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_OPTIONAL, "prop", "ws"),
    _STX__AFTER_ZERO_OR_N,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 2) _STX_TERM_LITERAL(2, "=", 1,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("'='", "prop")
  )
  if (ti == 3) _STX_TERM_RULE(ws,3,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_OPTIONAL, "prop", "ws"),
    _STX__AFTER_ZERO_OR_N,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 4) _STX_TERM_RULE(value,4,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_REQUIRED, "prop", "value"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("value", "prop")
  )
  if (ti == 5) _STX_TERM_RULE(eol,5,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_REQUIRED, "prop", "eol"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("eol", "prop")
  )
  else break;
,
 if (ti <= 0) _STX__ERROR_REQUIRED("word")
 else if (ti <= 2) _STX__ERROR_REQUIRED("'='")
 else if (ti <= 4) _STX__ERROR_REQUIRED("value")
 else if (ti <= 5) _STX__ERROR_REQUIRED("eol")
)

_STX_RULE(rule,RULE,
  if (ti == 0) _STX_TERM_RULE(word,0,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_REQUIRED, "rule", "word"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("word", "rule")
  )
  if (ti == 1) _STX_TERM_RULE(ws,1,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_OPTIONAL, "rule", "ws"),
    _STX__AFTER_ZERO_OR_N,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 2) _STX_TERM_LITERAL(2, "-", 1,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("'-'", "rule")
  )
  if (ti == 3) _STX_TERM_LITERAL(3, ">", 1,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("'>'", "rule")
  )
  if (ti == 4) _STX_TERM_RULE(ws,4,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_OPTIONAL, "rule", "ws"),
    _STX__AFTER_ZERO_OR_N,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 5) _STX_TERM_RULE(word,5,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_REQUIRED, "rule", "word"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("word", "rule")
  )
  if (ti == 6) _STX_TERM_RULE(ws,6,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_OPTIONAL, "rule", "ws"),
    _STX__AFTER_ZERO_OR_N,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 7) _STX_TERM_LITERAL(7, ":", 1,
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("':'", "rule")
  )
  if (ti == 8) _STX_TERM_RULE(ws,8,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_OPTIONAL, "rule", "ws"),
    _STX__AFTER_ZERO_OR_N,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 9) _STX_TERM_RULE(eol,9,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_REQUIRED, "rule", "eol"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("eol", "rule")
  )
  else break;
,
 if (ti <= 0) _STX__ERROR_REQUIRED("word")
 else if (ti <= 2) _STX__ERROR_REQUIRED("'-'")
 else if (ti <= 3) _STX__ERROR_REQUIRED("'>'")
 else if (ti <= 5) _STX__ERROR_REQUIRED("word")
 else if (ti <= 7) _STX__ERROR_REQUIRED("':'")
 else if (ti <= 9) _STX__ERROR_REQUIRED("eol")
)

_STX_RULE(declaration,RULE,
  if (ti == 0) _STX_TERM_RULE(rule,0,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_REQUIRED, "declaration", "rule"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("rule", "declaration")
  )
  if (ti == 1) _STX_TERM_RULE(declaration_1,1,
    _STX__ON_ERROR_NONFAIL(RULE,_STX__ON_ERROR_MOD_MANY(1, "Group", false), "declaration", "Group"),
    _STX__AFTER_ONE_OR_N,
    _STX__ELSE_MANY(1)
    _STX__ELSE_REQUIRED_STOP("Group", "declaration")
  )
  else break;
,
 if (ti <= 0) _STX__ERROR_REQUIRED("rule")
 else if (ti <= 1) _STX__ERROR_REQUIRED("Group")
)

_STX_RULE(declaration_1,GROUP,
  if (ti == 0) _STX_TERM_RULE(ws,0,
    _STX__ON_ERROR_NONFAIL(GROUP,_STX__ON_ERROR_MOD_OPTIONAL, "declaration_1", "ws"),
    _STX__AFTER_ZERO_OR_N,
    _STX__ELSE_OPTIONAL()
  )
  if (ti == 1) _STX_TERM_RULE(prop,1,
    _STX__ON_ERROR_NONFAIL(GROUP,_STX__ON_ERROR_MOD_REQUIRED, "declaration_1", "prop"),
    _STX__AFTER_REQUIRED,
    _STX__ELSE_REQUIRED_STOP("prop", "declaration_1")
  )
  else break;
,
 if (ti <= 1) _STX__ERROR_REQUIRED("prop")
)

_STX_RULE(grammar,GRAMMAR,
  if (ti == 0) _STX_TERM_RULE(declaration,0,
    _STX__ON_ERROR_NONFAIL(GRAMMAR,_STX__ON_ERROR_MOD_MANY(0, "declaration", true), "grammar", "declaration"),
    _STX__AFTER_ONE_OR_N,
    _STX__ELSE_MANY(0)
    _STX__ELSE_REQUIRED_STOP("declaration", "grammar")
  )
  else break;
,
 if (ti <= 0) _STX__ERROR_REQUIRED("declaration")
)

const CSTNode semantics::parse(std::string def) {
  auto node_ptr = parse_grammar(def, def.size(), 0, 0);
  auto node = *node_ptr;
  delete node_ptr;
  return node;
}
