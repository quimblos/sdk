#pragma once
#include "semantics.h"

qb_suite(test_semantics, "semantics", {

    qb_describe("Parser", {
    
        qb_test("grammar -> (error: incomplete rule)", {
            std::string input = "grammar ->\n";
            auto root = semantics::parse(input);
            // std::cout << root.to_str(input) << std::endl;
            qb_assert(root.errors.size() == 1)
        })

        qb_test("grammar -> Grammar: (error: no prop)", {
            std::string input = "grammar -> Grammar:\n";
            auto root = semantics::parse(input);
            // std::cout << root.to_str(input) << std::endl;
            qb_assert(root.errors.size() == 1)
        })

        qb_test("grammar -> Grammar:\n  msg = #word", {
            std::string input = "grammar -> Grammar:\n  msg = #word\n";
            auto root = semantics::parse(input);
            // std::cout << root.to_str(input) << std::endl;
            qb_assert(root.errors.size() == 0)
            qb_assert(root.children.size() == 1)
            qb_assert(root.children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].rule == "declaration")
            qb_assert(root.children[0].children.size() == 2)
            qb_assert(root.children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].rule == "rule")
            qb_assert(root.children[0].children[0].children.size() == 8)
            qb_assert(root.children[0].children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].children[0].rule == "word")
            qb_assert(root.children[0].children[0].children[0].text(input) == "grammar")
            qb_assert(root.children[0].children[0].children[5].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].children[5].rule == "word")
            qb_assert(root.children[0].children[0].children[5].text(input) == "Grammar")
            qb_assert(root.children[0].children[1].kind == CSTNode::Kind::GROUP)
            qb_assert(root.children[0].children[1].children.size() == 3)
            qb_assert(root.children[0].children[1].children[2].children.size() == 6)
            qb_assert(root.children[0].children[1].children[2].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[1].children[2].children[0].rule == "word")
            qb_assert(root.children[0].children[1].children[2].children[0].text(input) == "msg")
            qb_assert(root.children[0].children[1].children[2].children[4].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[1].children[2].children[4].rule == "value")
            qb_assert(root.children[0].children[1].children[2].children[4].text(input) == "#word")
        })

        qb_test("grammar -> Grammar:\n  msg = #word\n  tone = tone", {
            std::string input = "grammar -> Grammar:\n  msg = #word\n  tone = tone\n";
            auto root = semantics::parse(input);
            // std::cout << root.to_str(input) << std::endl;
            qb_assert(root.errors.size() == 0)
            qb_assert(root.children.size() == 1)
            qb_assert(root.children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].rule == "declaration")
            qb_assert(root.children[0].children.size() == 3)
            qb_assert(root.children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].rule == "rule")
            qb_assert(root.children[0].children[0].children.size() == 8)
            qb_assert(root.children[0].children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].children[0].rule == "word")
            qb_assert(root.children[0].children[0].children[0].text(input) == "grammar")
            qb_assert(root.children[0].children[0].children[5].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].children[5].rule == "word")
            qb_assert(root.children[0].children[0].children[5].text(input) == "Grammar")
            qb_assert(root.children[0].children[1].kind == CSTNode::Kind::GROUP)
            qb_assert(root.children[0].children[1].children.size() == 3)
            qb_assert(root.children[0].children[1].children[2].children.size() == 6)
            qb_assert(root.children[0].children[1].children[2].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[1].children[2].children[0].rule == "word")
            qb_assert(root.children[0].children[1].children[2].children[0].text(input) == "msg")
            qb_assert(root.children[0].children[1].children[2].children[4].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[1].children[2].children[4].rule == "value")
            qb_assert(root.children[0].children[1].children[2].children[4].text(input) == "#word")
            qb_assert(root.children[0].children[2].kind == CSTNode::Kind::GROUP)
            qb_assert(root.children[0].children[2].children.size() == 3)
            qb_assert(root.children[0].children[2].children[2].children.size() == 6)
            qb_assert(root.children[0].children[2].children[2].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[2].children[2].children[0].rule == "word")
            qb_assert(root.children[0].children[2].children[2].children[0].text(input) == "tone")
            qb_assert(root.children[0].children[2].children[2].children[4].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[2].children[2].children[4].rule == "value")
            qb_assert(root.children[0].children[2].children[2].children[4].text(input) == "tone")
        })

        qb_test("grammar -> Grammar:\n  msg = msg\nmsg -> Msg\n  text = #word", {
            std::string input = "grammar -> Grammar:\n  msg = msg\nmsg -> Msg\n  text = #word\n";
            auto root = semantics::parse(input);
            // std::cout << root.to_str(input) << std::endl;
            qb_assert(root.errors.size() == 1)
            qb_assert(root.children.size() == 2)
            qb_assert(root.children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].rule == "declaration")
            qb_assert(root.children[0].children.size() == 2)
            qb_assert(root.children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].rule == "rule")
            qb_assert(root.children[0].children[0].children.size() == 8)
            qb_assert(root.children[0].children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].children[0].rule == "word")
            qb_assert(root.children[0].children[0].children[0].text(input) == "grammar")
            qb_assert(root.children[0].children[0].children[5].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].children[5].rule == "word")
            qb_assert(root.children[0].children[0].children[5].text(input) == "Grammar")
            qb_assert(root.children[0].children[1].kind == CSTNode::Kind::GROUP)
            qb_assert(root.children[0].children[1].children.size() == 3)
            qb_assert(root.children[0].children[1].children[2].children.size() == 6)
            qb_assert(root.children[0].children[1].children[2].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[1].children[2].children[0].rule == "word")
            qb_assert(root.children[0].children[1].children[2].children[0].text(input) == "msg")
            qb_assert(root.children[0].children[1].children[2].children[4].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[1].children[2].children[4].rule == "value")
            qb_assert(root.children[0].children[1].children[2].children[4].text(input) == "msg")
            qb_assert(root.children[1].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[1].rule == "declaration")
            qb_assert(root.children[1].errors.size() == 1)
            qb_assert(root.children[1].children.size() == 1)
            qb_assert(root.children[1].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[1].children[0].rule == "rule")
            qb_assert(root.children[1].children[0].errors.size() == 1)
            qb_assert(root.children[1].children[0].children.size() == 6)
        })

        qb_test("grammar -> Grammar:\n  msg = msg\nmsg -> Msg:\n  text = #word", {
            std::string input = "grammar -> Grammar:\n  msg = msg\nmsg -> Msg:\n  text = #word\n";
            auto root = semantics::parse(input);
            // std::cout << root.to_str(input) << std::endl;
            qb_assert(root.errors.size() == 0)
            qb_assert(root.children.size() == 2)
            qb_assert(root.children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].rule == "declaration")
            qb_assert(root.children[0].children.size() == 2)
            qb_assert(root.children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].rule == "rule")
            qb_assert(root.children[0].children[0].children.size() == 8)
            qb_assert(root.children[0].children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].children[0].rule == "word")
            qb_assert(root.children[0].children[0].children[0].text(input) == "grammar")
            qb_assert(root.children[0].children[0].children[5].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[0].children[5].rule == "word")
            qb_assert(root.children[0].children[0].children[5].text(input) == "Grammar")
            qb_assert(root.children[0].children[1].kind == CSTNode::Kind::GROUP)
            qb_assert(root.children[0].children[1].children.size() == 3)
            qb_assert(root.children[0].children[1].children[2].children.size() == 6)
            qb_assert(root.children[0].children[1].children[2].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[1].children[2].children[0].rule == "word")
            qb_assert(root.children[0].children[1].children[2].children[0].text(input) == "msg")
            qb_assert(root.children[0].children[1].children[2].children[4].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[0].children[1].children[2].children[4].rule == "value")
            qb_assert(root.children[0].children[1].children[2].children[4].text(input) == "msg")
            qb_assert(root.children[1].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[1].rule == "declaration")
            qb_assert(root.children[1].children.size() == 2)
            qb_assert(root.children[1].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[1].children[0].rule == "rule")
            qb_assert(root.children[1].children[0].children.size() == 8)
            qb_assert(root.children[1].children[0].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[1].children[0].children[0].rule == "word")
            qb_assert(root.children[1].children[0].children[0].text(input) == "msg")
            qb_assert(root.children[1].children[0].children[5].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[1].children[0].children[5].rule == "word")
            qb_assert(root.children[1].children[0].children[5].text(input) == "Msg")
            qb_assert(root.children[1].children[1].kind == CSTNode::Kind::GROUP)
            qb_assert(root.children[1].children[1].children.size() == 3)
            qb_assert(root.children[1].children[1].children[2].children.size() == 6)
            qb_assert(root.children[1].children[1].children[2].children[0].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[1].children[1].children[2].children[0].rule == "word")
            qb_assert(root.children[1].children[1].children[2].children[0].text(input) == "text")
            qb_assert(root.children[1].children[1].children[2].children[4].kind == CSTNode::Kind::RULE)
            qb_assert(root.children[1].children[1].children[2].children[4].rule == "value")
            qb_assert(root.children[1].children[1].children[2].children[4].text(input) == "#word")
        })
    })
})