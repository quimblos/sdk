#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#pragma once

namespace ebnf {

    typedef std::vector<std::pair<uint8_t, uint8_t>> charmap_t;

    struct Term {
        enum Type {
            LITERAL = 0x00,
            CHARMAP,
            IDENTIFIER,
            GROUP,
            OR
        } type;

        std::string content;
        charmap_t charmap;
        std::vector<Term> children;

        enum Modifier {
            NONE = 0x00,
            OPTIONAL,
            ZERO_OR_N,
            ONE_OR_N
        } modifier;

        Term(Type type, std::string content, Modifier modifier = Modifier::NONE)
            : type(type),
              content(content),
              modifier(modifier) {}

        Term(Type type, std::vector<std::pair<uint8_t, uint8_t>> charmap, Modifier modifier = Modifier::NONE)
            : type(type),
              charmap(charmap),
              modifier(modifier) {}

        Term(Type type, std::vector<Term> children, Modifier modifier = Modifier::NONE)
            : type(type),
              children(children),
              modifier(modifier) {}
    };

    struct Rule {
        std::string name;
        std::vector<Term> terms;
    };

    typedef std::unordered_map<std::string, Rule> RuleMap;

    struct res_t {
        enum Code {
            OK = 0x00,
            UNEXPECTED_EOF,
            UNEXPECTED_EOL,
            EMPTY_RULE,
            EMPTY_LITERAL,
            EMPTY_CHARMAP,
            NO_GRAMMAR_RULE
        } code;
        const ebnf::RuleMap* rules = nullptr;
    };

    const res_t parse(std::string ebnf);

}