#pragma once
#include "ast.h"
#include "semantics/impl.h"

struct RuleText : public ASTNode {
    std::string text;
    RuleText(const CSTNode& cst): ASTNode(cst) {}
    const std::string to_str() const {
        return "";
    }
};

struct RuleTextOfFirst : public ASTNode {
    std::string text;
    RuleTextOfFirst(const CSTNode& cst): ASTNode(cst) {}
    const std::string to_str() const {
        return "";
    }
};

qb_suite(test_semantics_impl, "semantics > implementation", {

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
            auto ast = RuleText(cst);
            ast.text = "test"; // TODO: build instead of mock
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
            auto ast = RuleTextOfFirst(cst);
            ast.text = "test"; // TODO: build instead of mock
            qb_assert(ast.cst == &cst);
            qb_assert(ast.text == "test");
        })
   
    })
})