#include "parser.h"
#include "semantics.h"

const CSTNode* parse_eol(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_letter(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_word(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_ws(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_value(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_value_2(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_prop(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_rule(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_declaration(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_declaration_1(const std::string& input, uint32_t n, uint32_t i, uint8_t term);
const CSTNode* parse_grammar(const std::string& input, uint32_t n, uint32_t i, uint8_t term);

_RULE(eol,RULE,
  if (ti == 0) _TERM_RULE(ws,0,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_OPTIONAL, "eol", "ws"),
    __AFTER_ZERO_OR_N,
    __ELSE_OPTIONAL()
  )
  if (ti == 1) _TERM_CHARMAP(1,
    ch == 10 || ch == 13 || ch == 0,
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("[\\10\\13\\0]", "eol")
  )
  else break;
,
 if (ti <= 1) __ERROR_REQUIRED("[\\10\\13\\0]")
)

_RULE(letter,RULE,
  if (ti == 0) _TERM_CHARMAP(0,
    (ch >= 97 && ch <= 122) || (ch >= 65 && ch <= 90),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("[\\97-\\122\\65-\\90]", "letter")
  )
  else break;
,
 if (ti <= 0) __ERROR_REQUIRED("[\\97-\\122\\65-\\90]")
)

_RULE(word,RULE,
  if (ti == 0) _TERM_RULE(letter,0,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_MANY(0, "letter", false), "word", "letter"),
    __AFTER_ONE_OR_N,
    __ELSE_MANY(0)
    __ELSE_REQUIRED_STOP("letter", "word")
  )
  else break;
,
 if (ti <= 0) __ERROR_REQUIRED("letter")
)

_RULE(ws,RULE,
  if (ti == 0) _TERM_CHARMAP(0,
    ch == 160 || ch == 32 || ch == 9,
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("[\\160\\32\\9]", "ws")
  )
  else break;
,
 if (ti <= 0) __ERROR_REQUIRED("[\\160\\32\\9]")
)

_RULE(value,RULE,
  if (ti == 0) _TERM_LITERAL(0, "#", 1,
    __AFTER_OPTIONAL,
    __ELSE_OPTIONAL()
  )
  if (ti == 1) _TERM_RULE(word,1,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_REQUIRED, "value", "word"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("word", "value")
  )
  if (ti == 2) _TERM_RULE(value_2,2,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_OPTIONAL, "value", "Group"),
    __AFTER_OPTIONAL,
    __ELSE_OPTIONAL()
  )
  else break;
,
 if (ti <= 1) __ERROR_REQUIRED("word")
)

_RULE(value_2,GROUP,
  if (ti == 0) _TERM_LITERAL(0, "[", 1,
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("'['", "value_2")
  )
  if (ti == 1) _TERM_LITERAL(1, "]", 1,
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("']'", "value_2")
  )
  else break;
,
 if (ti <= 0) __ERROR_REQUIRED("'['")
 else if (ti <= 1) __ERROR_REQUIRED("']'")
)

_RULE(prop,RULE,
  if (ti == 0) _TERM_RULE(word,0,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_REQUIRED, "prop", "word"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("word", "prop")
  )
  if (ti == 1) _TERM_RULE(ws,1,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_OPTIONAL, "prop", "ws"),
    __AFTER_ZERO_OR_N,
    __ELSE_OPTIONAL()
  )
  if (ti == 2) _TERM_LITERAL(2, "=", 1,
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("'='", "prop")
  )
  if (ti == 3) _TERM_RULE(ws,3,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_OPTIONAL, "prop", "ws"),
    __AFTER_ZERO_OR_N,
    __ELSE_OPTIONAL()
  )
  if (ti == 4) _TERM_RULE(value,4,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_REQUIRED, "prop", "value"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("value", "prop")
  )
  if (ti == 5) _TERM_RULE(eol,5,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_REQUIRED, "prop", "eol"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("eol", "prop")
  )
  else break;
,
 if (ti <= 0) __ERROR_REQUIRED("word")
 else if (ti <= 2) __ERROR_REQUIRED("'='")
 else if (ti <= 4) __ERROR_REQUIRED("value")
 else if (ti <= 5) __ERROR_REQUIRED("eol")
)

_RULE(rule,RULE,
  if (ti == 0) _TERM_RULE(word,0,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_REQUIRED, "rule", "word"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("word", "rule")
  )
  if (ti == 1) _TERM_RULE(ws,1,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_OPTIONAL, "rule", "ws"),
    __AFTER_ZERO_OR_N,
    __ELSE_OPTIONAL()
  )
  if (ti == 2) _TERM_LITERAL(2, "-", 1,
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("'-'", "rule")
  )
  if (ti == 3) _TERM_LITERAL(3, ">", 1,
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("'>'", "rule")
  )
  if (ti == 4) _TERM_RULE(ws,4,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_OPTIONAL, "rule", "ws"),
    __AFTER_ZERO_OR_N,
    __ELSE_OPTIONAL()
  )
  if (ti == 5) _TERM_RULE(word,5,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_REQUIRED, "rule", "word"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("word", "rule")
  )
  if (ti == 6) _TERM_RULE(ws,6,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_OPTIONAL, "rule", "ws"),
    __AFTER_ZERO_OR_N,
    __ELSE_OPTIONAL()
  )
  if (ti == 7) _TERM_LITERAL(7, ":", 1,
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("':'", "rule")
  )
  if (ti == 8) _TERM_RULE(ws,8,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_OPTIONAL, "rule", "ws"),
    __AFTER_ZERO_OR_N,
    __ELSE_OPTIONAL()
  )
  if (ti == 9) _TERM_RULE(eol,9,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_REQUIRED, "rule", "eol"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("eol", "rule")
  )
  else break;
,
 if (ti <= 0) __ERROR_REQUIRED("word")
 else if (ti <= 2) __ERROR_REQUIRED("'-'")
 else if (ti <= 3) __ERROR_REQUIRED("'>'")
 else if (ti <= 5) __ERROR_REQUIRED("word")
 else if (ti <= 7) __ERROR_REQUIRED("':'")
 else if (ti <= 9) __ERROR_REQUIRED("eol")
)

_RULE(declaration,RULE,
  if (ti == 0) _TERM_RULE(rule,0,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_REQUIRED, "declaration", "rule"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("rule", "declaration")
  )
  if (ti == 1) _TERM_RULE(declaration_1,1,
    __ON_ERROR_NONFAIL(RULE,__ON_ERROR_MOD_MANY(1, "Group", false), "declaration", "Group"),
    __AFTER_ONE_OR_N,
    __ELSE_MANY(1)
    __ELSE_REQUIRED_STOP("Group", "declaration")
  )
  else break;
,
 if (ti <= 0) __ERROR_REQUIRED("rule")
 else if (ti <= 1) __ERROR_REQUIRED("Group")
)

_RULE(declaration_1,GROUP,
  if (ti == 0) _TERM_RULE(ws,0,
    __ON_ERROR_NONFAIL(GROUP,__ON_ERROR_MOD_OPTIONAL, "declaration_1", "ws"),
    __AFTER_ZERO_OR_N,
    __ELSE_OPTIONAL()
  )
  if (ti == 1) _TERM_RULE(prop,1,
    __ON_ERROR_NONFAIL(GROUP,__ON_ERROR_MOD_REQUIRED, "declaration_1", "prop"),
    __AFTER_REQUIRED,
    __ELSE_REQUIRED_STOP("prop", "declaration_1")
  )
  else break;
,
 if (ti <= 1) __ERROR_REQUIRED("prop")
)

_RULE(grammar,GRAMMAR,
  if (ti == 0) _TERM_RULE(declaration,0,
    __ON_ERROR_NONFAIL(GRAMMAR,__ON_ERROR_MOD_MANY(0, "declaration", true), "grammar", "declaration"),
    __AFTER_ONE_OR_N,
    __ELSE_MANY(0)
    __ELSE_REQUIRED_STOP("declaration", "grammar")
  )
  else break;
,
 if (ti <= 0) __ERROR_REQUIRED("declaration")
)

const CSTNode semantics::parse(std::string input) {
  auto node_ptr = parse_grammar(input, input.size(), 0, 0);
  auto node = *node_ptr;
  delete node_ptr;
  return node;
}
