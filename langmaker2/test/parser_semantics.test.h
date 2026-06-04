#pragma once
#include "ast.h"
#include "parser_semantics.h"

_AST_NODE(RuleText
,
    std::string text;
,
    _PROP_TEXT(text)
)

_AST_NODE(RuleTextOfFirst
,
    std::string text;
,
    _PROP_TEXT_OF_FIRST(text, 0)
)

qb_suite(test_parser_semantics, "parser_semantics", {

    qb_describe("Build AST Node", {
    
        qb_test("text", {
            std::string input = "test";
            auto cst = CSTNode({
                .kind = CSTNode::Kind::RULE,
                .rule = "rule",
                .term = 0,
                .start = 0,
                .end = 4
            });
            auto ast = RuleText(input, &cst);
            qb_assert(ast.cst == &cst);
            qb_assert(ast.text == "test");
        })
    
        qb_test("text of first", {
            std::string input = "test";
            auto cst = CSTNode({
                .kind = CSTNode::Kind::RULE,
                .rule = "rule",
                .term = 0,
                .start = 0,
                .end = 4,
                .children = {
                    CSTNode({
                        .kind = CSTNode::Kind::RULE,
                        .rule = "key",
                        .term = 0,
                        .start = 0,
                        .end = 4
                    })
                }
            });
            auto ast = RuleTextOfFirst(input, &cst);
            qb_assert(ast.cst == &cst);
            qb_assert(ast.text == "test");
        })
   
    })
})