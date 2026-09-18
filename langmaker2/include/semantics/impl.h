#pragma once

#include "ast.h"

/*
    AST Props
*/

#define _SMT_PROP_TEXT(NAME) \
    this->NAME = cst.text(input);

#define _SMT_PROP_TEXT_OF_FIRST(NAME, T) \
    _SMT_FIRST(NAME, cst, T); \
    this->NAME = _##NAME->text(input);

/*
    CST Parsers
*/

#define _SMT_FIRST(NAME, T, CLS) \
    for (const auto& term : cst.children) { \
        if (term.term == T) { this->NAME = new CLS(term, input); break; } \
    }

#define _SMT_ALL(NAME, T, CLS) \
    for (const auto& term : cst.children) { \
        if (term.term == T) { this->NAME.push_back(new CLS(term, input)); } \
    }

// #define _SMT_EVERY(NAME, PARENT, T, FN...) \
//     for (uint16_t i = 0; i < PARENT->children.size(); i++) { \
//         if (PARENT->children[i].term != T) continue; \
//         auto _##NAME = &PARENT->children[i]; \
//         FN \
//     }

// #define _SMT_ALL(NAME, PARENT, FN...) \
//     for (uint16_t i = 0; i < PARENT->children.size(); i++) { \
//         auto _##NAME = &PARENT->children[i]; \
//         FN \
//     }

// #define _SMT_FIRST_OF_GROUP(NAME, PARENT, T_GROUP, T) \
//     _SMT_FIRST(group_##NAME, PARENT, T_GROUP) \
//     _SMT_FIRST(NAME, _group_##NAME, T)
    