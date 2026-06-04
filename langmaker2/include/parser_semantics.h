#pragma once

#include "ast.h"

#define _AST_NODE(NAME, PROPS, CONSTRUCTOR) \
    struct NAME : public ASTNode { \
        PROPS \
        NAME(const std::string& input, const CSTNode* cst): ASTNode(cst) { \
            CONSTRUCTOR \
        } \
    };

/*
    AST Props
*/

#define _PROP_TEXT(NAME) \
    this->NAME = cst->text(input);

#define _PROP_TEXT_OF_FIRST(NAME, T) \
    _FIRST(NAME, cst, T); \
    this->NAME = _##NAME->text(input);

/*
    CST Parsers
*/

#define _FIRST(NAME, PARENT, T) \
    CSTNode* _##NAME; \
    for (const auto& term : PARENT->children) { \
        if (term.term == T) { _##NAME = (CSTNode*) &term; break; } \
    }

#define _EVERY(NAME, PARENT, T, FN...) \
    for (uint16_t i = 0; i < PARENT->children.size(); i++) { \
        if (PARENT->children[i].term != T) continue; \
        auto _##NAME = &PARENT->children[i]; \
        FN \
    }

#define _ALL(NAME, PARENT, FN...) \
    for (uint16_t i = 0; i < PARENT->children.size(); i++) { \
        auto _##NAME = &PARENT->children[i]; \
        FN \
    }
