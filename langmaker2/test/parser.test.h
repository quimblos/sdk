#pragma once
#include "cst.h"
#include "parser.h"

#define PARSE_RULE(INPUT, FN, GUARD) \
    std::string input = INPUT; \
    uint16_t n = input.size(); \
    uint16_t i = 0; \
    std::vector<CSTNode> children; \
    std::vector<Error> errors; \
    uint16_t ti = 0; \
    while (i < n) { \
        FN \
    } \
    GUARD \

_RULE(rule0_fail, RULE,
    if (ti == 0) _TERM_LITERAL(0, "abc", 3,
        __AFTER_REQUIRED,
        __ELSE_REQUIRED_FAIL()
    )
    else break;,
    if (ti <= 0) __ERROR_REQUIRED("'abc'")
)

_RULE(rule0_stop, RULE,
    if (ti == 0) _TERM_LITERAL(0, "abc", 3,
        __AFTER_REQUIRED,
        __ELSE_REQUIRED_STOP("'abc'", "rule0_stop")
    )
    else break;,
    if (ti <= 0) __ERROR_REQUIRED("'abc'")
)

_RULE(rule0_continue, RULE,
    if (ti == 0) _TERM_LITERAL(0, "abc", 3,
        __AFTER_REQUIRED,
        __ELSE_REQUIRED_CONTINUE
    )
    else break;,
    if (ti <= 0) __ERROR_REQUIRED("'abc'")
)

qb_suite(test_parser, "parser", {

    qb_describe("Check Literal", {
    
        qb_test("check_literal('test','test') == true", {
            bool match = parser::check_literal("test", 4, 0, "test");
            qb_assert(match == true)
        })

        qb_test("check_literal('test','te') == true", {
            bool match = parser::check_literal("test", 4, 0, "te");
            qb_assert(match == true)
        })

        qb_test("check_literal('te','test') == false", {
            bool match = parser::check_literal("te", 2, 0, "test");
            qb_assert(match == false)
        })

        qb_test("check_literal('abc test def','test') == false", {
            bool match = parser::check_literal("abc test def", 12, 0, "test");
            qb_assert(match == false)
        })

        qb_test("check_literal('abc test def','test') == true", {
            bool match = parser::check_literal("abc test def", 12, 4, "test");
            qb_assert(match == true)
        })
        
    })

    qb_describe("Parse Literal", {
    
        qb_test("'test' <-> 'test'", {
            PARSE_RULE(
                "test",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_REQUIRED,
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("'test'")
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 4)
            // qb_assert(children[0].text == "test")
        })
    
        qb_test("'te' <-> 'test'", {
            PARSE_RULE(
                "te",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_REQUIRED,
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("'test'")
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 1)
            qb_assert(errors[0].code == Error::Code::REQUIRED_TERM)
            qb_assert(errors[0].pos == 0)
        })
    })

    qb_describe("Parse CharMap", {
    
        qb_test("'x' <-> [abc]", {
            PARSE_RULE(
                "x",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_REQUIRED,
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("[abc]")
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 1)
            qb_assert(errors[0].code == Error::Code::REQUIRED_TERM)
            qb_assert(errors[0].pos == 0)
        })

        qb_test("'a' <-> [abc]", {
            PARSE_RULE(
                "a",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_REQUIRED,
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("[abc]")
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 1)
            // qb_assert(children[0].text == "a")
        })
    
        qb_test("'aaa' <-> [abc]", {
            PARSE_RULE(
                "aaa",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_REQUIRED,
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("[abc]")
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 1)
            // qb_assert(children[0].text == "a")
        })
    
    })

    qb_describe("Parse Rule", {
    
        qb_test("'a' <-> rule0_fail", {
            auto out = parse_rule0_fail("a", 1, 0, 0);
            qb_assert(out == nullptr)
        })

        qb_test("'a' <-> rule0_stop", {
            auto out = parse_rule0_stop("a", 1, 0, 0);
            qb_assert(out != nullptr)
            qb_assert(out->children.size() == 0)
            qb_assert(out->errors.size() == 1)
            delete out;
        })

        qb_test("'a' <-> rule0_continue", {
            auto out = parse_rule0_continue("a", 1, 0, 0);
            qb_assert(out != nullptr)
            qb_assert(out->children.size() == 0)
            qb_assert(out->errors.size() == 1)
            delete out;
        })

        qb_test("'abc' <-> rule0", {
            PARSE_RULE(
                "abc",
                if (ti == 0) _TERM_RULE(rule0_continue, 0,
                    __ON_ERROR_CONTINUE,
                    __AFTER_REQUIRED,
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("rule0")
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
        })
        
        qb_test("'a' <-> rule0 (continue)", {
            PARSE_RULE(
                "a",
                if (ti == 0) _TERM_RULE(rule0_continue, 0,
                    __ON_ERROR_CONTINUE,
                    __AFTER_REQUIRED,
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("rule0")
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 1)
            qb_assert(errors[0].code == Error::Code::REQUIRED_TERM)
            qb_assert(errors[0].pos == 0)
        })

    })

    qb_describe("Modifiers (Literal)", {
    
        qb_test("'test' <-> 'test'?", {
            PARSE_RULE(
                "test",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_OPTIONAL,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 4)
            // qb_assert(children[0].text == "test")
        })
    
        qb_test("'te' <-> 'test'?", {
            PARSE_RULE(
                "te",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_OPTIONAL,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 0)
        })
    
        qb_test("'testtest' <-> 'test'?", {
            PARSE_RULE(
                "testtest",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_OPTIONAL,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 4)
            // qb_assert(children[0].text == "test")
        })

        qb_test("'test' <-> 'test'*", {
            PARSE_RULE(
                "test",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_ZERO_OR_N,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 4)
            // qb_assert(children[0].text == "test")
        })
    
        qb_test("'te' <-> 'test'*", {
            PARSE_RULE(
                "te",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_ZERO_OR_N,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 0)
        })

        qb_test("'testtest' <-> 'test'*", {
            PARSE_RULE(
                "testtest",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_ZERO_OR_N,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 2)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 4)
            // qb_assert(children[0].text == "test")
            qb_assert(children[1].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[1].term == 0)
            qb_assert(children[1].start == 4)
            qb_assert(children[1].end == 8)
            // qb_assert(children[1].text == "test")
        })

        qb_test("'test' <-> 'test'+", {
            PARSE_RULE(
                "test",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_ONE_OR_N,
                    __ELSE_MANY(0)
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("'test'")
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 4)
            // qb_assert(children[0].text == "test")
        })

        qb_test("'te' <-> 'test'+", {
            PARSE_RULE(
                "te",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_ONE_OR_N,
                    __ELSE_MANY(0)
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("'test'")
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 1)
            qb_assert(errors[0].code == Error::Code::REQUIRED_TERM)
            qb_assert(errors[0].pos == 0)
        })

        qb_test("'testtest' <-> 'test'+", {
            PARSE_RULE(
                "testtest",
                if (ti == 0) _TERM_LITERAL(0, "test", 4,
                    __AFTER_ONE_OR_N,
                    __ELSE_MANY(0)
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("'test'")
            )
            qb_assert(children.size() == 2)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 4)
            // qb_assert(children[0].text == "test")
            qb_assert(children[1].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[1].term == 0)
            qb_assert(children[1].start == 4)
            qb_assert(children[1].end == 8)
            // qb_assert(children[1].text == "test")
        })
        
    })

    qb_describe("Modifiers (CharMap)", {
    
        qb_test("'x' <-> [abc]?", {
            PARSE_RULE(
                "x",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_OPTIONAL,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 0)
        })

        qb_test("'a' <-> [abc]?", {
            PARSE_RULE(
                "a",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_OPTIONAL,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 1)
            // qb_assert(children[0].text == "a")
        })
    
        qb_test("'aaa' <-> [abc]?", {
            PARSE_RULE(
                "aaa",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_OPTIONAL,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 1)
            // qb_assert(children[0].text == "a")
        })
    
        qb_test("'x' <-> [abc]*", {
            PARSE_RULE(
                "x",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_ZERO_OR_N,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 0)
        })
    
        qb_test("'a' <-> [abc]*", {
            PARSE_RULE(
                "a",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_ZERO_OR_N,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 1)
            // qb_assert(children[0].text == "a")
        })
    
        qb_test("'aaa' <-> [abc]*", {
            PARSE_RULE(
                "aaa",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_ZERO_OR_N,
                    __ELSE_OPTIONAL()
                )
                else break;,
            )
            qb_assert(children.size() == 3)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 1)
            // qb_assert(children[0].text == "a")
            qb_assert(children[1].term == 0)
            qb_assert(children[1].start == 1)
            qb_assert(children[1].end == 2)
            // qb_assert(children[1].text == "a")
            qb_assert(children[2].term == 0)
            qb_assert(children[2].start == 2)
            qb_assert(children[2].end == 3)
            // qb_assert(children[2].text == "a")
        })
    
        qb_test("'x' <-> [abc]+", {
            PARSE_RULE(
                "x",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_ONE_OR_N,
                    __ELSE_MANY(0)
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("'test'")
            )
            qb_assert(children.size() == 0)
            qb_assert(errors.size() == 1)
            qb_assert(errors[0].code == Error::Code::REQUIRED_TERM)
            qb_assert(errors[0].pos == 0)
        })
    
        qb_test("'a' <-> [abc]+", {
            PARSE_RULE(
                "a",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_ONE_OR_N,
                    __ELSE_MANY(0)
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("'test'")
            )
            qb_assert(children.size() == 1)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].kind == CSTNode::Kind::LITERAL)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 1)
            // qb_assert(children[0].text == "a")
        })
    
        qb_test("'aaa' <-> [abc]+", {
            PARSE_RULE(
                "aaa",
                if (ti == 0) _TERM_CHARMAP(0,
                    ch == 'a' || ch == 'b' || ch == 'c', 
                    __AFTER_ONE_OR_N,
                    __ELSE_MANY(0)
                    __ELSE_REQUIRED_CONTINUE
                )
                else break;,
                if (ti <= 0) __ERROR_REQUIRED("'test'")
            )
            qb_assert(children.size() == 3)
            qb_assert(errors.size() == 0)
            qb_assert(children[0].term == 0)
            qb_assert(children[0].start == 0)
            qb_assert(children[0].end == 1)
            // qb_assert(children[0].text == "a")
            qb_assert(children[1].term == 0)
            qb_assert(children[1].start == 1)
            qb_assert(children[1].end == 2)
            // qb_assert(children[1].text == "a")
            qb_assert(children[2].term == 0)
            qb_assert(children[2].start == 2)
            qb_assert(children[2].end == 3)
            // qb_assert(children[2].text == "a")
        })
   
    })
})