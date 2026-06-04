#include <string>
#include <vector>
#include "ebnf.h"

#define __RETURN(KIND, NAME) \
    return new CSTNode({ \
        .kind = CSTNode::Kind::KIND, \
        .rule = NAME, \
        .term = term, \
        .start = start, \
        .end = i, \
        .children = children, \
        .errors = errors, \
    }); \

#define _RULE(NAME, KIND, FN, GUARD) \
    const CSTNode* parse_##NAME(const std::string& input, uint32_t n, uint32_t i, uint8_t term) { \
        std::vector<CSTNode> children; \
        std::vector<Error> errors; \
        uint32_t start = i; \
        uint8_t ti = 0; \
        while (i < n) { \
            FN \
        } \
        GUARD \
        __RETURN(KIND, #NAME) \
    }

// Terms

#define _TERM_LITERAL(TERM, CONTENT, CONTENT_LEN, AFTER, ELSE) { \
    bool match = parser::check_literal(input, n, i, CONTENT); \
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

#define _TERM_CHARMAP(TERM, MATCH, AFTER, ELSE) { \
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

#define _TERM_RULE(RULE, TERM, ON_ERROR, AFTER, ELSE) { \
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

#define __ON_ERROR_FAIL \
    if (node == nullptr || node->errors.size()) return nullptr;
#define __ON_ERROR_STOP(CPPKIND, MODIFIER, RULE, TERM) \
    if (node->errors.size()) { \
        MODIFIER \
        children.push_back(*node); \
        delete node; \
        __ERROR_INNER(TERM) \
        __RETURN(CPPKIND, RULE) \
    }
#define __ON_ERROR_CONTINUE 

#define __ON_ERROR_STOP_OPTIONAL \
    delete node; \
    ti++; \
    continue;

#define __ON_ERROR_STOP_MANY(TERM) \
    if (children.size() > 0 && children.back().term == TERM) { \
        delete node; \
        ti++; \
        continue; \
    } \

#define __ON_ERROR_OR \
    if (node == nullptr || node->errors.size()) { \
        delete node; \
        ti += 2; \
        continue; \
    } \

// After

#define __AFTER_REQUIRED ti++; continue;
#define __AFTER_OPTIONAL ti++; continue;
#define __AFTER_ZERO_OR_N continue;
#define __AFTER_ONE_OR_N if (i>=n) ti++; continue;

#define __AFTER_OR \
    ti++; \
    break;

// Else

    // NONE
#define __ELSE_REQUIRED_FAIL() \
    return nullptr;

#define __ELSE_REQUIRED_STOP(TERM, RULE_NAME) \
    __ERROR_REQUIRED(TERM) \
    __RETURN(RULE, RULE_NAME)

#define __ELSE_REQUIRED_CONTINUE \
    break;
    
    // ?/*
#define __ELSE_OPTIONAL() \
    ti++; \
    continue;

    // */+
#define __ELSE_MANY(TERM) \
    if (children.size() > 0 && children.back().term == TERM) { \
        ti++; \
        continue; \
    } \

#define __ELSE_OR \
    ti += 2; \
    continue;

// Else

#define __ERROR_REQUIRED(TERM) \
    errors.push_back({ \
        .code = Error::Code::REQUIRED_TERM, \
        .pos = i, \
        .message = TERM " is required" \
    });

#define __ERROR_INNER(TERM) \
    errors.push_back({ \
        .code = Error::Code::REQUIRED_TERM, \
        .pos = i, \
        .message = TERM " contains errors" \
    });

namespace parser {
    bool check_literal(std::string input, uint32_t n, uint32_t i, std::string literal);
}