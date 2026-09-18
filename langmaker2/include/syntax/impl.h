#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define _STX__RETURN(KIND, NAME) \
    return new CSTNode({ \
        .kind = CSTNode::Kind::KIND, \
        .rule = NAME, \
        .term = term, \
        .start = start, \
        .end = i, \
        .children = children, \
        .errors = errors, \
    }); \

#define _STX_RULE(NAME, KIND, FN, GUARD) \
    const CSTNode* parse_##NAME(const std::string& input, uint32_t n, uint32_t i, uint8_t term) { \
        std::vector<CSTNode> children; \
        std::vector<CSTError> errors; \
        uint32_t start = i; \
        uint8_t ti = 0; \
        while (i < n) { \
            FN \
        } \
        GUARD \
        _STX__RETURN(KIND, #NAME) \
    }

// Terms

#define _STX_TERM_LITERAL(TERM, CONTENT, CONTENT_LEN, AFTER, ELSE) { \
    bool match = syntax::check_literal(input, n, i, CONTENT); \
    if (match) { \
        children.push_back({ \
            .kind = CSTNode::Kind::LITERAL, \
            .term = TERM, \
            .start = i, \
            .end = i + (uint32_t)CONTENT_LEN \
        }); \
        i += CONTENT_LEN; \
        AFTER \
    } \
    else { \
        ELSE \
    } \
}

#define _STX_TERM_CHARMAP(TERM, MATCH, AFTER, ELSE) { \
    const char ch = input[i]; \
    if (MATCH) { \
        children.push_back({ \
            .kind = CSTNode::Kind::LITERAL, \
            .term = TERM, \
            .start = i, \
            .end = i + (uint32_t)1 \
        }); \
        i++; \
        AFTER \
    } \
    else { \
        ELSE \
    } \
}

#define _STX_TERM_RULE(RULE, TERM, ON_ERROR, AFTER, ELSE) { \
    const CSTNode* node = parse_##RULE(input, n, i, TERM); \
    ON_ERROR \
    if (node->end > i) { \
        children.push_back(*node); \
        i = node->end; \
        delete node; \
        AFTER \
    } \
    else { \
        delete node; \
        ELSE \
    } \
}

// After

#define _STX___ON_ERROR_FAIL \
    if (node == nullptr || node->errors.size()) return nullptr;
#define _STX__ON_ERROR_NONFAIL(CPPKIND, MODIFIER, RULE, TERM) \
    if (node->errors.size()) { \
        MODIFIER \
        children.push_back(*node); \
        delete node; \
        _STX__ERROR_INNER(TERM) \
        _STX__RETURN(CPPKIND, RULE) \
    }

#define _STX__ON_ERROR_MOD_REQUIRED \
    i = node->end;
    
#define _STX__ON_ERROR_MOD_OPTIONAL \
    delete node; \
    ti++; \
    continue;

#define _STX__ON_ERROR_MOD_MANY(T, TERM, IS_GRAMMAR) \
    if (children.size() > 0 && children.back().term == T) { \
        if (IS_GRAMMAR) {\
            children.push_back(*node); \
            i = node->end; \
            _STX__ERROR_INNER(TERM) \
        } \
        delete node; \
        ti++; \
        continue; \
    }

#define _STX__ON_ERROR_OR \
    if (node == nullptr || node->errors.size()) { \
        delete node; \
        ti += 2; \
        continue; \
    }

// After

#define _STX__AFTER_REQUIRED ti++; continue;
#define _STX__AFTER_OPTIONAL ti++; continue;
#define _STX__AFTER_ZERO_OR_N continue;
#define _STX__AFTER_ONE_OR_N if (i>=n) ti++; continue;

#define _STX__AFTER_OR \
    ti++; \
    break;

// Else

    // NONE
#define _STX__ELSE_REQUIRED_FAIL() \
    return nullptr;

#define _STX__ELSE_REQUIRED_STOP(TERM, RULE_NAME) \
    _STX__ERROR_REQUIRED(TERM) \
    _STX__RETURN(RULE, RULE_NAME)

#define _STX__ELSE_REQUIRED_CONTINUE \
    break;
    
    // ?/*
#define _STX__ELSE_OPTIONAL() \
    ti++; \
    continue;

    // */+
#define _STX__ELSE_MANY(TERM) \
    if (children.size() > 0 && children.back().term == TERM) { \
        ti++; \
        continue; \
    } \

#define _STX__ELSE_OR \
    ti += 2; \
    continue;

// Else

#define _STX__ERROR_REQUIRED(TERM) \
    errors.push_back({ \
        .code = CSTError::Code::REQUIRED_TERM, \
        .pos = i, \
        .pi = (uint16_t)(children.size()-1), \
        .ti = (uint8_t) ti, \
        .message = TERM " is required" \
    });

#define _STX__ERROR_INNER(TERM) \
    errors.push_back({ \
        .code = CSTError::Code::REQUIRED_TERM, \
        .pos = i, \
        .pi = (uint16_t)(children.size()-1), \
        .ti = (uint8_t) ti, \
        .message = TERM " contains errors" \
    });

namespace syntax {
    bool check_literal(std::string input, uint32_t n, uint32_t i, std::string literal);
}