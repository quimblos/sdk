#define __INLINE_CST "#pragma once\n" \
"\n" \
"#include <cstdint>\n" \
"#include <sstream>\n" \
"#include <string>\n" \
"#include <vector>\n" \
"\n" \
"#define COLOR_CYAN \"\\033[36m\"\n" \
"#define COLOR_YELLOW \"\\033[33m\"\n" \
"#define COLOR_PURPLE \"\\033[35m\"\n" \
"#define COLOR_GRAY \"\\033[30m\"\n" \
"#define COLOR_RED \"\\033[31m\"\n" \
"#define COLOR_NC \"\\033[0m\"\n" \
"\n" \
"struct CSTError {\n" \
"    enum Code {\n" \
"        REQUIRED_TERM = 0x00,\n" \
"        PARSING_FAILED\n" \
"    } code;\n" \
"    uint32_t pos;\n" \
"    uint16_t pi;\n" \
"    uint8_t ti;\n" \
"    std::string message;\n" \
"};\n" \
"\n" \
"struct CSTNode {\n" \
"    enum Kind {\n" \
"        GRAMMAR = 0x00,\n" \
"        LITERAL,\n" \
"        GROUP,\n" \
"        RULE\n" \
"    } kind;\n" \
"    std::string rule;\n" \
"    \n" \
"    uint8_t term;\n" \
"    uint32_t start;\n" \
"    uint32_t end;\n" \
"    \n" \
"    std::vector<CSTNode> children;\n" \
"    std::vector<CSTError> errors;\n" \
"\n" \
"    // For identation-sensitive languages\n" \
"    uint8_t tab = 0;\n" \
"    \n" \
"    std::string text(std::string file) const {\n" \
"        return file.substr(this->start, this->end-this->start);\n" \
"    }\n" \
"\n" \
"    std::string to_str(std::string file, uint16_t depth = 0) const {\n" \
"        std::ostringstream ss;\n" \
"        if (this->errors.size()) ss << COLOR_RED << \"!\" << COLOR_NC;\n" \
"        else  ss << \" \";\n" \
"\n" \
"\n" \
"        std::string input_format = std::string(this->end-this->start, ' ');\n" \
"        for (uint16_t i = this->start; i < this->end; i++) {\n" \
"            if (file[i] == '\\n') input_format[i-this->start] = '\\\\';\n" \
"            else input_format[i-this->start] = file[i];\n" \
"        }\n" \
"\n" \
"        for (uint16_t i = 0; i < depth; i++) {\n" \
"            ss << \" \";\n" \
"        }\n" \
"        switch (this->kind) {\n" \
"            case Kind::GRAMMAR: ss << COLOR_CYAN << \"GRAMMAR\" << COLOR_NC; break;\n" \
"            case Kind::LITERAL: ss << COLOR_GRAY << \"LITERAL\"; break;\n" \
"            case Kind::GROUP: ss << COLOR_YELLOW << \"GROUP\" << COLOR_NC; break;\n" \
"            case Kind::RULE: ss << COLOR_PURPLE << \"RULE(\" << this->rule << \")\"  << COLOR_NC; break;\n" \
"        }\n" \
"        ss << \" \" << +this->term << \" [\" << this->start << \":\" << this->end << \"] \" << COLOR_GRAY << \"'\" << input_format << \"'\" << COLOR_NC << \"\\n\";\n" \
"        \n" \
"        for (const auto& error : this->errors) {\n" \
"            for (uint16_t i = 0; i < depth+1; i++) {\n" \
"                ss << COLOR_RED << \" \";\n" \
"            }\n" \
"            ss << \"[\" << error.pi << \"] t\" << +error.ti << \" : \" << error.message << \"\\n\" << COLOR_NC;\n" \
"        }\n" \
"\n" \
"        for (const auto& child : this->children) {\n" \
"            if (child.kind == CSTNode::Kind::LITERAL) continue;\n" \
"            if (child.rule == \"letter\") continue;\n" \
"            if (child.rule == \"ws\") continue;\n" \
"            if (child.rule == \"eol\") continue;\n" \
"            ss << child.to_str(file, depth+1);\n" \
"        }\n" \
"        return ss.str();\n" \
"    }\n" \
"};"
#define __INLINE_AST "#pragma once\n" \
"\n" \
"#include <vector>\n" \
"#include \"cst.h\"\n" \
"\n" \
"struct ASTNode {\n" \
"  const CSTNode* cst;\n" \
"  ASTNode(const CSTNode& cst): cst(&cst) {}\n" \
"\n" \
"  virtual const std::string to_str() const = 0;\n" \
"};"
#define __INLINE_UTIL_STRING "#include <string>\n" \
"\n" \
"const std::string pad(const std::string& input, const std::string& pad = \"  \", const std::string& pad0 = \"\") {\n" \
"    auto str = std::string(input);\n" \
"    size_t i = 0;\n" \
"    while (i < str.size()-1) {\n" \
"        if (str[i] == '\\n') {\n" \
"            str.replace(i+1, 0, pad);\n" \
"            i += pad.length() + 1;\n" \
"        }\n" \
"        else i++;\n" \
"    }\n" \
"    return pad0 + str;\n" \
"}"
#define __INLINE_SYNTAX_IMPL "#pragma once\n" \
"\n" \
"#include <cstdint>\n" \
"#include <string>\n" \
"#include <vector>\n" \
"\n" \
"#define _STX__RETURN(KIND, NAME) \\\n" \
"    return new CSTNode({ \\\n" \
"        .kind = CSTNode::Kind::KIND, \\\n" \
"        .rule = NAME, \\\n" \
"        .term = term, \\\n" \
"        .start = start, \\\n" \
"        .end = i, \\\n" \
"        .children = children, \\\n" \
"        .errors = errors, \\\n" \
"    }); \\\n" \
"\n" \
"#define _STX_RULE(NAME, KIND, FN, GUARD) \\\n" \
"    const CSTNode* parse_##NAME(const std::string& input, uint32_t n, uint32_t i, uint8_t term) { \\\n" \
"        std::vector<CSTNode> children; \\\n" \
"        std::vector<CSTError> errors; \\\n" \
"        uint32_t start = i; \\\n" \
"        uint8_t ti = 0; \\\n" \
"        while (i < n) { \\\n" \
"            FN \\\n" \
"        } \\\n" \
"        GUARD \\\n" \
"        _STX__RETURN(KIND, #NAME) \\\n" \
"    }\n" \
"\n" \
"// Terms\n" \
"\n" \
"#define _STX_TERM_LITERAL(TERM, CONTENT, CONTENT_LEN, AFTER, ELSE) { \\\n" \
"    bool match = syntax::check_literal(input, n, i, CONTENT); \\\n" \
"    if (match) { \\\n" \
"        children.push_back({ \\\n" \
"            .kind = CSTNode::Kind::LITERAL, \\\n" \
"            .term = TERM, \\\n" \
"            .start = i, \\\n" \
"            .end = i + (uint32_t)CONTENT_LEN \\\n" \
"        }); \\\n" \
"        i += CONTENT_LEN; \\\n" \
"        AFTER \\\n" \
"    } \\\n" \
"    else { \\\n" \
"        ELSE \\\n" \
"    } \\\n" \
"}\n" \
"\n" \
"#define _STX_TERM_CHARMAP(TERM, MATCH, AFTER, ELSE) { \\\n" \
"    const char ch = input[i]; \\\n" \
"    if (MATCH) { \\\n" \
"        children.push_back({ \\\n" \
"            .kind = CSTNode::Kind::LITERAL, \\\n" \
"            .term = TERM, \\\n" \
"            .start = i, \\\n" \
"            .end = i + (uint32_t)1 \\\n" \
"        }); \\\n" \
"        i++; \\\n" \
"        AFTER \\\n" \
"    } \\\n" \
"    else { \\\n" \
"        ELSE \\\n" \
"    } \\\n" \
"}\n" \
"\n" \
"#define _STX_TERM_RULE(RULE, TERM, ON_ERROR, AFTER, ELSE) { \\\n" \
"    const CSTNode* node = parse_##RULE(input, n, i, TERM); \\\n" \
"    ON_ERROR \\\n" \
"    if (node->end > i) { \\\n" \
"        children.push_back(*node); \\\n" \
"        i = node->end; \\\n" \
"        delete node; \\\n" \
"        AFTER \\\n" \
"    } \\\n" \
"    else { \\\n" \
"        delete node; \\\n" \
"        ELSE \\\n" \
"    } \\\n" \
"}\n" \
"\n" \
"// After\n" \
"\n" \
"#define _STX___ON_ERROR_FAIL \\\n" \
"    if (node == nullptr || node->errors.size()) return nullptr;\n" \
"#define _STX__ON_ERROR_NONFAIL(CPPKIND, MODIFIER, RULE, TERM) \\\n" \
"    if (node->errors.size()) { \\\n" \
"        MODIFIER \\\n" \
"        children.push_back(*node); \\\n" \
"        delete node; \\\n" \
"        _STX__ERROR_INNER(TERM) \\\n" \
"        _STX__RETURN(CPPKIND, RULE) \\\n" \
"    }\n" \
"\n" \
"#define _STX__ON_ERROR_MOD_REQUIRED \\\n" \
"    i = node->end;\n" \
"    \n" \
"#define _STX__ON_ERROR_MOD_OPTIONAL \\\n" \
"    delete node; \\\n" \
"    ti++; \\\n" \
"    continue;\n" \
"\n" \
"#define _STX__ON_ERROR_MOD_MANY(T, TERM, IS_GRAMMAR) \\\n" \
"    if (children.size() > 0 && children.back().term == T) { \\\n" \
"        if (IS_GRAMMAR) {\\\n" \
"            children.push_back(*node); \\\n" \
"            i = node->end; \\\n" \
"            _STX__ERROR_INNER(TERM) \\\n" \
"        } \\\n" \
"        delete node; \\\n" \
"        ti++; \\\n" \
"        continue; \\\n" \
"    }\n" \
"\n" \
"#define _STX__ON_ERROR_OR \\\n" \
"    if (node == nullptr || node->errors.size()) { \\\n" \
"        delete node; \\\n" \
"        ti += 2; \\\n" \
"        continue; \\\n" \
"    }\n" \
"\n" \
"// After\n" \
"\n" \
"#define _STX__AFTER_REQUIRED ti++; continue;\n" \
"#define _STX__AFTER_OPTIONAL ti++; continue;\n" \
"#define _STX__AFTER_ZERO_OR_N continue;\n" \
"#define _STX__AFTER_ONE_OR_N if (i>=n) ti++; continue;\n" \
"\n" \
"#define _STX__AFTER_OR \\\n" \
"    ti++; \\\n" \
"    break;\n" \
"\n" \
"// Else\n" \
"\n" \
"    // NONE\n" \
"#define _STX__ELSE_REQUIRED_FAIL() \\\n" \
"    return nullptr;\n" \
"\n" \
"#define _STX__ELSE_REQUIRED_STOP(TERM, RULE_NAME) \\\n" \
"    _STX__ERROR_REQUIRED(TERM) \\\n" \
"    _STX__RETURN(RULE, RULE_NAME)\n" \
"\n" \
"#define _STX__ELSE_REQUIRED_CONTINUE \\\n" \
"    break;\n" \
"    \n" \
"    // ?/*\n" \
"#define _STX__ELSE_OPTIONAL() \\\n" \
"    ti++; \\\n" \
"    continue;\n" \
"\n" \
"    // */+\n" \
"#define _STX__ELSE_MANY(TERM) \\\n" \
"    if (children.size() > 0 && children.back().term == TERM) { \\\n" \
"        ti++; \\\n" \
"        continue; \\\n" \
"    } \\\n" \
"\n" \
"#define _STX__ELSE_OR \\\n" \
"    ti += 2; \\\n" \
"    continue;\n" \
"\n" \
"// Else\n" \
"\n" \
"#define _STX__ERROR_REQUIRED(TERM) \\\n" \
"    errors.push_back({ \\\n" \
"        .code = CSTError::Code::REQUIRED_TERM, \\\n" \
"        .pos = i, \\\n" \
"        .pi = (uint16_t)(children.size()-1), \\\n" \
"        .ti = (uint8_t) ti, \\\n" \
"        .message = TERM \" is required\" \\\n" \
"    });\n" \
"\n" \
"#define _STX__ERROR_INNER(TERM) \\\n" \
"    errors.push_back({ \\\n" \
"        .code = CSTError::Code::REQUIRED_TERM, \\\n" \
"        .pos = i, \\\n" \
"        .pi = (uint16_t)(children.size()-1), \\\n" \
"        .ti = (uint8_t) ti, \\\n" \
"        .message = TERM \" contains errors\" \\\n" \
"    });\n" \
"\n" \
"namespace syntax {\n" \
"    bool check_literal(std::string input, uint32_t n, uint32_t i, std::string literal);\n" \
"}"
#define __INLINE_SEMANTICS_IMPL "#pragma once\n" \
"\n" \
"#include \"ast.h\"\n" \
"\n" \
"/*\n" \
"    AST Props\n" \
"*/\n" \
"\n" \
"#define _SMT_PROP_TEXT(NAME) \\\n" \
"    this->NAME = cst.text(input);\n" \
"\n" \
"#define _SMT_PROP_TEXT_OF_FIRST(NAME, T) \\\n" \
"    _SMT_FIRST(NAME, cst, T); \\\n" \
"    this->NAME = _##NAME->text(input);\n" \
"\n" \
"/*\n" \
"    CST Parsers\n" \
"*/\n" \
"\n" \
"#define _SMT_FIRST(NAME, T, CLS) \\\n" \
"    for (const auto& term : cst.children) { \\\n" \
"        if (term.term == T) { this->NAME = new CLS(term, input); break; } \\\n" \
"    }\n" \
"\n" \
"#define _SMT_ALL(NAME, T, CLS) \\\n" \
"    for (const auto& term : cst.children) { \\\n" \
"        if (term.term == T) { this->NAME.push_back(new CLS(term, input)); } \\\n" \
"    }\n" \
"\n" \
"// #define _SMT_EVERY(NAME, PARENT, T, FN...) \\\n" \
"//     for (uint16_t i = 0; i < PARENT->children.size(); i++) { \\\n" \
"//         if (PARENT->children[i].term != T) continue; \\\n" \
"//         auto _##NAME = &PARENT->children[i]; \\\n" \
"//         FN \\\n" \
"//     }\n" \
"\n" \
"// #define _SMT_ALL(NAME, PARENT, FN...) \\\n" \
"//     for (uint16_t i = 0; i < PARENT->children.size(); i++) { \\\n" \
"//         auto _##NAME = &PARENT->children[i]; \\\n" \
"//         FN \\\n" \
"//     }\n" \
"\n" \
"// #define _SMT_FIRST_OF_GROUP(NAME, PARENT, T_GROUP, T) \\\n" \
"//     _SMT_FIRST(group_##NAME, PARENT, T_GROUP) \\\n" \
"//     _SMT_FIRST(NAME, _group_##NAME, T)\n" \
"    "
