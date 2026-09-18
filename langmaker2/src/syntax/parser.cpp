#include "syntax/parser.h"

// #define DEBUG_SYNTAX

#define _STX_ERROR(ERROR) \
    if (rule != nullptr) delete rule; \
    if (stack.size() > 0) delete stack.front(); \
    return { \
        .code = syntax::res_t::Code::ERROR, \
        .schema = schema \
    }; \

#define IS_SPACE(CHAR) (CHAR == ' ' | CHAR == '\t')
#define IS_ALPHANUMERIC(CHAR) (CHAR >= 'A' && CHAR <= 'z') || (CHAR >= '0' && CHAR <= '9')

#define _STX_ADD_TERM(TYPE, COL0, LENGTH) \
    stack.front()->children.emplace_back( \
        syntax::Term::Type::TYPE, \
        def.substr(COL0, LENGTH) \
    ); \
    state = ParserState::RULE;

#define _STX_ADD_CHARMAP_TERM(TYPE, COL0, LENGTH) \
    stack.front()->children.emplace_back( \
        syntax::Term::Type::TYPE, \
        parse_charmap(def.substr(COL0, LENGTH)) \
    ); \
    state = ParserState::RULE;

#define _STX_ADD_RULE() \
    rule->terms = stack.front()->children; \
    delete stack.front(); \
    stack.clear(); \
    if (rule->terms.size() == 0) { \
        _STX_ERROR(EMPTY_RULE) \
    } \
    schema->emplace(std::pair(rule->name, *rule)); \
    delete rule; \
    state = ParserState::VOID;

#ifdef DEBUG_SYNTAX
    #include <iostream>
#endif

enum ParserState {
    VOID = 0x00,
    COMMENT,
    RULE_NAME,
    RULE,
    LITERAL,
    CHARMAP,
    IDENTIFIER,
};

uint8_t parse_hex(const char ch0, const char ch1) {
    uint8_t val = 0;
    if (ch1 >= '0' && ch1 <= '9') val += ch1 - '0';
    else if (ch1 >= 'A' && ch1 <= 'F') val += ch1 - 'A' + 10;
    else if (ch1 >= 'a' && ch1 <= 'f') val += ch1 - 'a' + 10;
    if (ch0 >= '0' && ch0 <= '9') val += 16*(ch0 - '0');
    else if (ch0 >= 'A' && ch0 <= 'F') val += 16*(ch0 - 'A' + 10);
    else if (ch0 >= 'a' && ch0 <= 'f') val += 16*(ch0 - 'A' + 10);
    return val;
}

std::vector<std::pair<uint8_t, uint8_t>> parse_charmap(const std::string content) {
#ifdef DEBUG_SYNTAX
    std::cout << "<<parse charmap '" << content << "'>>" << std::endl;
#endif
    std::vector<std::pair<uint8_t, uint8_t>> pairs;
    for (size_t i = 0; i < content.size(); i++) {
        char cur = content[i];
        if (cur == '#' && i < content.size()-3 && content[i+1] == 'x') {
            char hex = parse_hex(content[i+2], content[i+3]);
            pairs.push_back({hex, hex});
#ifdef DEBUG_SYNTAX
            std::cout << "<<add to charmap " << +((uint8_t)hex) << "(" << (char)hex << ")" << ">>" << std::endl;
#endif
            i += 3;
        }
        else if (i < content.size()-2 && content[i+1] == '-') {
            pairs.push_back({cur, content[i+2]});
#ifdef DEBUG_SYNTAX
            std::cout << "<<add to charmap " << +((uint8_t)cur) << "(" << (char)cur << ")" << " ~ " << +((uint8_t)content[i+2]) << "(" << (char)content[i+2] << ")" << ">>" << std::endl;
#endif
            i += 2;
        }
        else {
            pairs.push_back({cur, cur});
#ifdef DEBUG_SYNTAX
            std::cout << "<<add to charmap " << +((uint8_t)cur) << "(" << (char)cur << ")" << ">>" << std::endl;
#endif
        }
    }
    return pairs;
}

const syntax::res_t syntax::parse(std::string def) {

    auto schema = new syntax::Schema();

    size_t col0 = 0;
    size_t name_end = 0;
    auto state = ParserState::VOID;
    
    syntax::Rule* rule = nullptr;
    auto stack = std::list<syntax::Term*>();

    auto n = def.size();
    for (size_t col = 0; col < n; col++) {

        char ch0 = def[col];

#ifdef DEBUG_SYNTAX
        if (ch0 == '\n') std::cout << "\\n";
        else  std::cout << " " << ch0;
        std::cout << " " << col << " ";
        switch (state) {
            case ParserState::VOID: std::cout << "VOID"; break;
            case ParserState::COMMENT: std::cout << "COMMENT"; break;
            case ParserState::RULE_NAME: std::cout << "RULE_NAME"; break;
            case ParserState::RULE: std::cout << "RULE"; break;
            case ParserState::LITERAL: std::cout << "LITERAL"; break;
            case ParserState::CHARMAP: std::cout << "CHARMAP"; break;
            case ParserState::IDENTIFIER: std::cout << "IDENTIFIER"; break;
        }
        std::cout << std::endl;
#endif

        // New line
        if (ch0 == '\n') {
            // If void, ignore
            if (state == ParserState::VOID) continue;
            // If comment, ignore
            if (state == ParserState::COMMENT) continue;

            // If state is not rule, a rule is malformed
            if (state != ParserState::RULE) {
                _STX_ERROR(UNEXPECTED_EOL)
            }
            _STX_ADD_RULE()
#ifdef DEBUG_SYNTAX
                std::cout << "<end rule>" << std::endl;
#endif
            continue;
        }

        char ch1 = col < n-1 ? def[col+1] : '\0';

        // VOID

        if (state == ParserState::VOID) {
            col0 = col;
            // [space or tab]: stay blank
            if (std::isspace(ch0)) continue;
            // [/*]: start comment
            if (ch0 == '/' && ch1 == '*') {
                state = ParserState::COMMENT;
                col++; // *
#ifdef DEBUG_SYNTAX
                std::cout << " " << ch1 << " " << col << std::endl;
                std::cout << "<start comment>" << std::endl;
#endif
                continue;
            }
            // [A~z or 0-9]: start rule
            if (IS_ALPHANUMERIC(ch0)) {
                state = ParserState::RULE_NAME;
                name_end = 0;
#ifdef DEBUG_SYNTAX
                std::cout << "<start rule name>" << std::endl;
#endif
            }
        }

        // COMMENT

        else if (state == ParserState::COMMENT) {
            // [*/]: end comment
            if (ch0 == '*' && ch1 == '/') {
                state = ParserState::VOID;
                col++; // /
#ifdef DEBUG_SYNTAX
                std::cout << " " << ch1 << " " << col << std::endl;
                std::cout << "<end comment>" << std::endl;
#endif
            }
            continue;
        }

        // RULE_NAME

        else if (state == ParserState::RULE_NAME) {
            char ch2 = col < n-2 ? def[col+2] : '\0';
            
            if (std::isspace(ch0)) {
                name_end = col;
#ifdef DEBUG_SYNTAX
                std::cout << "<end rule name>" << std::endl;
#endif
            }
            // [::=]: start rule
            if (ch0 == ':' && ch1 == ':' && ch2 == '=') {
                auto end = name_end == 0 ? col : name_end;
                rule = new syntax::Rule({
                    .name = def.substr(col0, end-col0)
                });
                stack.push_front(new syntax::Term(syntax::Term::Type::GROUP, (std::vector<syntax::Term>) {}));
                state = ParserState::RULE;
                col += 2; // :=
#ifdef DEBUG_SYNTAX
                std::cout << " " << ch1 << " " << col << std::endl;
                std::cout << " " << ch2 << " " << col << std::endl;
                std::cout << "<start rule>" << std::endl;
#endif
            }
        }

        // RULE

        else if (state == ParserState::RULE) {
            col0 = col;
            // [']: start literal
            if (ch0 == '\'') {
                state = ParserState::LITERAL;
#ifdef DEBUG_SYNTAX
                std::cout << "<start literal>" << std::endl;
#endif
            }
            // [[]: start charmap
            else if (ch0 == '[') {
                state = ParserState::CHARMAP;
#ifdef DEBUG_SYNTAX
                std::cout << "<start charmap>" << std::endl;
#endif
            }
            // [(]: start group
            else if (ch0 == '(') {
                stack.front()->children.emplace_back(
                    syntax::Term::Type::GROUP,
                    (std::vector<syntax::Term>) {}
                );
                stack.push_front(&stack.front()->children.back());
#ifdef DEBUG_SYNTAX
                std::cout << "<start group (" << stack.size()-1 << ")>" << std::endl;
#endif
            }
            // [)]: end group
            else if (ch0 == ')') {
                stack.pop_front();
#ifdef DEBUG_SYNTAX
                std::cout << "<end group (" << stack.size() << ")>" << std::endl;
#endif
            }
            // [|]: or term
            else if (ch0 == '|') {
                stack.front()->children.emplace_back(
                    syntax::Term::Type::OR,
                    ""
                );
#ifdef DEBUG_SYNTAX
                std::cout << "<or>" << std::endl;
#endif
            }
            // [A~z or 0-9]: start identifier
            else if (IS_ALPHANUMERIC(ch0)) {
                state = ParserState::IDENTIFIER;
#ifdef DEBUG_SYNTAX
                std::cout << "<start identifier>" << std::endl;
#endif
            }
        }

        // LITERAL

        else if (state == ParserState::LITERAL) {
            bool is_special_char = col > 0 && def[col-1] == '\\';
            if (is_special_char) {
                col0++;
            }
            // [']: end literal
            if (ch0 == '\'' && !is_special_char) {
                auto length = col-1-col0;
                if (length == 0) {
                    _STX_ERROR(EMPTY_LITERAL)
                }
                _STX_ADD_TERM(LITERAL, col0+1, length)
#ifdef DEBUG_SYNTAX
                std::cout << "<end literal = " << def.substr(col0+1, col-1-col0) << ">" << std::endl;
#endif
            }
        }

        // CHARMAP

        else if (state == ParserState::CHARMAP) {
            // []]: end charmap
            if (ch0 == ']') {
                auto length = col-1-col0;
                if (length == 0) {
                    _STX_ERROR(EMPTY_CHARMAP)
                }
                _STX_ADD_CHARMAP_TERM(CHARMAP, col0+1, length)
#ifdef DEBUG_SYNTAX
                std::cout << "<end charmap = " << def.substr(col0+1, col-1-col0) << ">" << std::endl;
#endif
            }
        }

        // IDENTIFIER
        if (state == ParserState::IDENTIFIER) {
            // end identifier
            if (std::isspace(ch1) || ch1 == '\n' || ch1 == '\0' || ch1 == ')' || ch1 == '?' || ch1 == '*' || ch1 == '+') {
                _STX_ADD_TERM(IDENTIFIER, col0, col+1-col0)
#ifdef DEBUG_SYNTAX
                std::cout << "<end identifier = " << def.substr(col0, col+1-col0) << ">" << std::endl;
#endif
            }
        }

        // MODIFIERS

        if (ch1 == '?') {
            stack.front()->children.back().modifier = syntax::Term::Modifier::OPTIONAL;
            col++;
#ifdef DEBUG_SYNTAX
                std::cout << "<add optional modifier>" << std::endl;
#endif
        }
        else if (ch1 == '*') {
            stack.front()->children.back().modifier = syntax::Term::Modifier::ZERO_OR_N;
            col++;
#ifdef DEBUG_SYNTAX
                std::cout << "<add zero_or_n modifier>" << std::endl;
#endif
        }
        else if (ch1 == '+') {
            stack.front()->children.back().modifier = syntax::Term::Modifier::ONE_OR_N;
            col++;
#ifdef DEBUG_SYNTAX
                std::cout << "<add one_or_n modifier>" << std::endl;
#endif
        }

    }
    
    if (state == ParserState::RULE) {
        _STX_ADD_RULE()
    }

    if (state != ParserState::VOID) {
        _STX_ERROR(UNEXPECTED_EOF)
    }

    // schema->emplace(std::pair("q", rule));

    return {
        .code = syntax::res_t::Code::OK,
        .schema = schema
    };

}