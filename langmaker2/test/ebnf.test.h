#pragma once
#include "ebnf.h"

#define ASSERT_RULES(RULES...) { \
    std::string rules[] = {RULES}; \
    qb_assert(res.rules->size() == sizeof(rules)/sizeof(std::string)) \
    for (const auto& rule : rules) { \
        qb_assert(res.rules->contains(rule)) \
        qb_assert(res.rules->at(rule).name == rule) \
    } \
}

qb_suite(test_ebnf, "ebnf", {

    qb_describe("Comment", {
    
        qb_test("[UNEXPECTED_EOF] /*", {
            auto res = ebnf::parse("/*");
            qb_assert(res.code == ebnf::res_t::Code::UNEXPECTED_EOF);
            delete res.rules;
        })
    
        qb_test("/* comment */'", {
            auto res = ebnf::parse("/* comment */");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            qb_assert(res.rules->size() == 0);
            delete res.rules;
        })

        qb_test("  /* comment */  '", {
            auto res = ebnf::parse("  /* comment */  ");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            qb_assert(res.rules->size() == 0);
            delete res.rules;
        })

        qb_test("  /* co\\nmm  \\nent */  '", {
            auto res = ebnf::parse("  /* co\nmm  \nent */  ");
            std::cout << +res.code << std::endl;
            qb_assert(res.code == ebnf::res_t::Code::OK);
            qb_assert(res.rules->size() == 0);
            delete res.rules;
        })
    })

    qb_describe("Rule", {
    
        qb_test("[UNEXPECTED_EOF] rule", {
            auto res = ebnf::parse("rule");
            qb_assert(res.code == ebnf::res_t::Code::UNEXPECTED_EOF);
            delete res.rules;
        })
    
        qb_test("[UNEXPECTED_EOL] rule\\n", {
            auto res = ebnf::parse("rule\n");
            qb_assert(res.code == ebnf::res_t::Code::UNEXPECTED_EOL);
            delete res.rules;
        })
    
        qb_test("[EMPTY_RULE] rule ::=", {
            auto res = ebnf::parse("rule ::=");
            qb_assert(res.code == ebnf::res_t::Code::EMPTY_RULE);
            delete res.rules;
        })
    
        qb_test("[EMPTY_RULE] rule ::= \\n", {
            auto res = ebnf::parse("rule ::= \n");
            qb_assert(res.code == ebnf::res_t::Code::EMPTY_RULE);
            delete res.rules;
        })
    
        qb_test("[UNEXPECTED_EOF] rule ::= 'lite", {
            auto res = ebnf::parse("rule ::= 'lite");
            qb_assert(res.code == ebnf::res_t::Code::UNEXPECTED_EOF);
            delete res.rules;
        })

        qb_test("[UNEXPECTED_EOL] rule ::= 'lite\\n", {
            auto res = ebnf::parse("rule ::= 'lite\n");
            LOG(res.code)
            qb_assert(res.code == ebnf::res_t::Code::UNEXPECTED_EOL);
            delete res.rules;
        })

        qb_test("[UNEXPECTED_EOF] rule1 ::= 'a' \\nrule2 ::= 'b'\\n rule3 ::= ", {
            auto res = ebnf::parse("'rule1 ::= 'a' \nrule2 ::= 'b'\n rule3 ::= '");
            qb_assert(res.code == ebnf::res_t::Code::UNEXPECTED_EOF);
            qb_assert(res.rules->size() == 2);
            delete res.rules;
        })
        
        qb_test("[UNEXPECTED_EOL] rule1 ::= 'a' \\nrule2 ::= 'b'\\n rule3 ::= '\\n", {
            auto res = ebnf::parse("'rule1 ::= 'a' \nrule2 ::= 'b'\n rule3 ::= '\n");
            qb_assert(res.code == ebnf::res_t::Code::UNEXPECTED_EOL);
            qb_assert(res.rules->size() == 2);
            delete res.rules;
        })

        qb_test("[EMPTY_RULE] rule1 ::= 'a' \\nrule2 ::= \\n rule3 ::= 'c", {
            auto res = ebnf::parse("'rule1 ::= 'a' \nrule2 ::= \n rule3 ::= 'c'");
            qb_assert(res.code == ebnf::res_t::Code::EMPTY_RULE);
            qb_assert(res.rules->size() == 1);
            delete res.rules;
        })
        
        qb_test("rule ::= 'a' 'b' 'c'", {
            auto res = ebnf::parse("'rule ::= 'a' 'b' 'c'");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            qb_assert(res.rules->size() == 1);
            qb_assert(res.rules->at("rule").terms.size() == 3);
            delete res.rules;
        })

        qb_test("rule1 ::= 'a' \\nrule2 ::= 'b'\\n rule3 ::= 'c'", {
            auto res = ebnf::parse("'rule1 ::= 'a' \nrule2 ::= 'b'\n rule3 ::= 'c'");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            qb_assert(res.rules->size() == 3);
            qb_assert(res.rules->at("rule1").terms.size() == 1);
            qb_assert(res.rules->at("rule2").terms.size() == 1);
            qb_assert(res.rules->at("rule3").terms.size() == 1);
            delete res.rules;
        })
    })

    qb_describe("Literal", {
        
        qb_test("[EMPTY_LITERAL] rule ::= ''", {
            auto res = ebnf::parse("rule ::= ''");
            qb_assert(res.code == ebnf::res_t::Code::EMPTY_LITERAL);
            delete res.rules;
        })

        qb_test("rule ::= 'literal'", {
            auto res = ebnf::parse("rule ::= 'literal'");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "literal")
            delete res.rules;
        })
    
        qb_test("rule ::= '\\''", {
            auto res = ebnf::parse("rule ::= '\\''");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "'")
            delete res.rules;
        })    
    })

    qb_describe("CharMap", {
        
        qb_test("[EMPTY_CHARMAP] rule ::= []", {
            auto res = ebnf::parse("rule ::= []");
            qb_assert(res.code == ebnf::res_t::Code::EMPTY_CHARMAP);
            delete res.rules;
        })

        qb_test("rule ::= [abc]", {
            auto res = ebnf::parse("rule ::= [abc]");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)

            std::vector<std::pair<uint8_t, uint8_t>> expected = {
                {'a','a'},
                {'b','b'},
                {'c','c'},
            };
            qb_assert(res.rules->at("rule").terms[0].charmap == expected)
            delete res.rules;
        })

        qb_test("rule ::= [a-c]", {
            auto res = ebnf::parse("rule ::= [a-c]");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)

            std::vector<std::pair<uint8_t, uint8_t>> expected = {
                {'a','c'}
            };
            qb_assert(res.rules->at("rule").terms[0].charmap == expected)
            delete res.rules;
        })
        qb_test("rule ::= [-c]", {
            auto res = ebnf::parse("rule ::= [-c]");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)

            std::vector<std::pair<uint8_t, uint8_t>> expected = {
                {'-','-'},
                {'c','c'}
            };
            qb_assert(res.rules->at("rule").terms[0].charmap == expected)
            delete res.rules;
        })
        qb_test("rule ::= [a-]", {
            auto res = ebnf::parse("rule ::= [a-]");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)

            std::vector<std::pair<uint8_t, uint8_t>> expected = {
                {'a','a'},
                {'-','-'},
            };
            qb_assert(res.rules->at("rule").terms[0].charmap == expected)
            delete res.rules;
        })
        qb_test("rule ::= [a-c0-4]", {
            auto res = ebnf::parse("rule ::= [a-c0-4]");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)

            std::vector<std::pair<uint8_t, uint8_t>> expected = {
                {'a','c'},
                {'0','4'},
            };
            qb_assert(res.rules->at("rule").terms[0].charmap == expected)
            delete res.rules;
        })
        qb_test("rule ::= [#x20]", {
            auto res = ebnf::parse("rule ::= [#x20]");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)

            std::vector<std::pair<uint8_t, uint8_t>> expected = {
                {' ',' '},
            };
            qb_assert(res.rules->at("rule").terms[0].charmap == expected)
            delete res.rules;
        })
        qb_test("rule ::= [#x20#x09]", {
            auto res = ebnf::parse("rule ::= [#x20#x09]");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)

            std::vector<std::pair<uint8_t, uint8_t>> expected = {
                {' ',' '},
                {'\t','\t'},
            };
            qb_assert(res.rules->at("rule").terms[0].charmap == expected)
            delete res.rules;
        })
    })

    qb_describe("Group", {
        
        qb_test("rule ::= ('a')", {
            auto res = ebnf::parse("rule ::= ('a')");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule").terms[0].children.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].children[0].content == "a")
            delete res.rules;
        })
        qb_test("rule ::= ('a' 'b' 'c')", {
            auto res = ebnf::parse("rule ::= ('a' 'b' 'c')");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 1)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule").terms[0].children.size() == 3)
            qb_assert(res.rules->at("rule").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].children[1].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].children[1].content == "b")
            qb_assert(res.rules->at("rule").terms[0].children[2].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].children[2].content == "c")
            delete res.rules;
        })
        qb_test("rule ::= ('a' 'b') 'c' 'd'", {
            auto res = ebnf::parse("rule ::= ('a' 'b') 'c' 'd'");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 3)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule").terms[0].children.size() == 2)
            qb_assert(res.rules->at("rule").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].children[1].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].children[1].content == "b")
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[1].content == "c")
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[2].content == "d")
            delete res.rules;
        })
     
    })

    qb_describe("Identifier", {
        
        qb_test("rule1 ::= rule2", {
            auto res = ebnf::parse("rule1 ::= rule2");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule1")
            qb_assert(res.rules->at("rule1").terms.size() == 1)
            qb_assert(res.rules->at("rule1").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule1").terms[0].content == "rule2")
            delete res.rules;
        })
        
        qb_test("rule1 ::= rule2   ", {
            auto res = ebnf::parse("rule1 ::= rule2   ");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule1")
            qb_assert(res.rules->at("rule1").terms.size() == 1)
            qb_assert(res.rules->at("rule1").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule1").terms[0].content == "rule2")
            delete res.rules;
        })

        qb_test("rule1 ::= rule2\\n", {
            auto res = ebnf::parse("rule1 ::= rule2\n");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule1")
            qb_assert(res.rules->at("rule1").terms.size() == 1)
            qb_assert(res.rules->at("rule1").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule1").terms[0].content == "rule2")
            delete res.rules;
        })
     
        qb_test("rule1 ::= rule2 rule3", {
            auto res = ebnf::parse("rule1 ::= rule2 rule3");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule1")
            qb_assert(res.rules->at("rule1").terms.size() == 2)
            qb_assert(res.rules->at("rule1").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule1").terms[0].content == "rule2")
            qb_assert(res.rules->at("rule1").terms[1].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule1").terms[1].content == "rule3")
            delete res.rules;
        })
    })

    qb_describe("Or", {
        
        qb_test("rule ::= 'a' | 'b'", {
            auto res = ebnf::parse("rule ::= 'a' | 'b'");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 3)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::OR)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[2].content == "b")
            delete res.rules;
        })
        
    })

    qb_describe("Complex", {
        
        qb_test("rule ::= 'a' | [b] | ('c'|'d') | e", {
            auto res = ebnf::parse("rule ::= 'a' | [b] | ('c'|'d') | e");
            qb_assert(res.code == ebnf::res_t::Code::OK);
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms.size() == 7)
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::OR)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::CHARMAP)
            std::vector<std::pair<uint8_t, uint8_t>> expected = {
                {'b','b'}
            };
            qb_assert(res.rules->at("rule").terms[2].charmap == expected)
            qb_assert(res.rules->at("rule").terms[3].type == ebnf::Term::Type::OR)
            qb_assert(res.rules->at("rule").terms[4].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule").terms[4].children.size() == 3)
            qb_assert(res.rules->at("rule").terms[4].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[4].children[0].content == "c")
            qb_assert(res.rules->at("rule").terms[4].children[1].type == ebnf::Term::Type::OR)
            qb_assert(res.rules->at("rule").terms[4].children[2].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[4].children[2].content == "d")
            qb_assert(res.rules->at("rule").terms[5].type == ebnf::Term::Type::OR)
            qb_assert(res.rules->at("rule").terms[6].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule").terms[6].content == "e")
            delete res.rules;
        })
        
    })

    qb_describe("Modifier (Literal)", {
        
        qb_test("rule ::= 'a'?", {
            auto res = ebnf::parse("rule ::= 'a'?");
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })
        
        qb_test("rule ::= 'a'? 'b' 'c'?", {
            auto res = ebnf::parse("rule ::= 'a'? 'b' 'c'?");
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[1].content == "b")
            qb_assert(res.rules->at("rule").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[2].content == "c")
            qb_assert(res.rules->at("rule").terms[2].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })
        
        qb_test("rule ::= 'a'*", {
            auto res = ebnf::parse("rule ::= 'a'*");
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })
        
        qb_test("rule ::= 'a'* 'b' 'c'*", {
            auto res = ebnf::parse("rule ::= 'a'* 'b' 'c'*");
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[1].content == "b")
            qb_assert(res.rules->at("rule").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[2].content == "c")
            qb_assert(res.rules->at("rule").terms[2].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })
        
        qb_test("rule ::= 'a'+", {
            auto res = ebnf::parse("rule ::= 'a'+");
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
        
        qb_test("rule ::= 'a'+ 'b' 'c'+", {
            auto res = ebnf::parse("rule ::= 'a'+ 'b' 'c'+");
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[1].content == "b")
            qb_assert(res.rules->at("rule").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[2].content == "c")
            qb_assert(res.rules->at("rule").terms[2].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })

        qb_test("rule ::= 'a'? 'b'* 'c'+", {
            auto res = ebnf::parse("rule ::= 'a'? 'b'* 'c'+");
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[0].content == "a")
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[1].content == "b")
            qb_assert(res.rules->at("rule").terms[1].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule").terms[2].content == "c")
            qb_assert(res.rules->at("rule").terms[2].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
    })

    qb_describe("Modifier (CharMap)", {
        
        qb_test("rule ::= [abc]?", {
            auto res = ebnf::parse("rule ::= [abc]?");
            const ebnf::charmap_t charmap = {{'a', 'a'}, {'b', 'b'}, {'c', 'c'}};
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[0].charmap == charmap)
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })
        
        qb_test("rule ::= [a]? [b] [c]?", {
            auto res = ebnf::parse("rule ::= [a]? [b] [c]?");
            const ebnf::charmap_t charmap_0 = {{'a', 'a'}};
            const ebnf::charmap_t charmap_1 = {{'b', 'b'}};
            const ebnf::charmap_t charmap_2 = {{'c', 'c'}};
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[0].charmap == charmap_0)
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[1].charmap == charmap_1)
            qb_assert(res.rules->at("rule").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[2].charmap == charmap_2)
            qb_assert(res.rules->at("rule").terms[2].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })
                
        qb_test("rule ::= [abc]*", {
            auto res = ebnf::parse("rule ::= [abc]*");
            const ebnf::charmap_t charmap = {{'a', 'a'}, {'b', 'b'}, {'c', 'c'}};
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[0].charmap == charmap)
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })
        
        qb_test("rule ::= [a]* [b] [c]*", {
            auto res = ebnf::parse("rule ::= [a]* [b] [c]*");
            const ebnf::charmap_t charmap_0 = {{'a', 'a'}};
            const ebnf::charmap_t charmap_1 = {{'b', 'b'}};
            const ebnf::charmap_t charmap_2 = {{'c', 'c'}};
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[0].charmap == charmap_0)
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[1].charmap == charmap_1)
            qb_assert(res.rules->at("rule").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[2].charmap == charmap_2)
            qb_assert(res.rules->at("rule").terms[2].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })
                
        qb_test("rule ::= [abc]+", {
            auto res = ebnf::parse("rule ::= [abc]+");
            const ebnf::charmap_t charmap = {{'a', 'a'}, {'b', 'b'}, {'c', 'c'}};
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[0].charmap == charmap)
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
        
        qb_test("rule ::= [a]+ [b] [c]+", {
            auto res = ebnf::parse("rule ::= [a]+ [b] [c]+");
            const ebnf::charmap_t charmap_0 = {{'a', 'a'}};
            const ebnf::charmap_t charmap_1 = {{'b', 'b'}};
            const ebnf::charmap_t charmap_2 = {{'c', 'c'}};
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[0].charmap == charmap_0)
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[1].charmap == charmap_1)
            qb_assert(res.rules->at("rule").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[2].charmap == charmap_2)
            qb_assert(res.rules->at("rule").terms[2].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
        
        qb_test("rule ::= [a]? [b]* [c]+", {
            auto res = ebnf::parse("rule ::= [a]? [b]* [c]+");
            const ebnf::charmap_t charmap_0 = {{'a', 'a'}};
            const ebnf::charmap_t charmap_1 = {{'b', 'b'}};
            const ebnf::charmap_t charmap_2 = {{'c', 'c'}};
            ASSERT_RULES("rule")
            qb_assert(res.rules->at("rule").terms[0].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[0].charmap == charmap_0)
            qb_assert(res.rules->at("rule").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            qb_assert(res.rules->at("rule").terms[1].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[1].charmap == charmap_1)
            qb_assert(res.rules->at("rule").terms[1].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            qb_assert(res.rules->at("rule").terms[2].type == ebnf::Term::Type::CHARMAP)
            qb_assert(res.rules->at("rule").terms[2].charmap == charmap_2)
            qb_assert(res.rules->at("rule").terms[2].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
    })

    qb_describe("Modifier (Identifier)", {
        
        qb_test("rule0 ::= rule1?", {
            auto res = ebnf::parse("rule0 ::= rule1?");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })
        
        qb_test("rule0 ::= rule1? rule2 rule3?", {
            auto res = ebnf::parse("rule0 ::= rule1? rule2 rule3?");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[1].content == "rule2")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[2].content == "rule3")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })
        
        qb_test("rule0 ::= rule1*", {
            auto res = ebnf::parse("rule0 ::= rule1*");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })
        
        qb_test("rule0 ::= rule1* rule2 rule3*", {
            auto res = ebnf::parse("rule0 ::= rule1* rule2 rule3*");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[1].content == "rule2")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[2].content == "rule3")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })
        
        qb_test("rule0 ::= rule1+", {
            auto res = ebnf::parse("rule0 ::= rule1+");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
        
        qb_test("rule0 ::= rule1+ rule2 rule3+", {
            auto res = ebnf::parse("rule0 ::= rule1+ rule2 rule3+");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[1].content == "rule2")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[2].content == "rule3")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
          
        qb_test("rule0 ::= rule1? rule2* rule3+", {
            auto res = ebnf::parse("rule0 ::= rule1? rule2* rule3+");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[1].content == "rule2")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[2].content == "rule3")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
    })

    qb_describe("Modifier (Group)", {
        
        qb_test("rule0 ::= ('a')", {
            auto res = ebnf::parse("rule0 ::= ('a')");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 1)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::NONE)
            delete res.rules;
        })
        
        qb_test("rule0 ::= ('a')?", {
            auto res = ebnf::parse("rule0 ::= ('a')?");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 1)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })
        
        qb_test("rule0 ::= ('a')? ('b') ('c')?", {
            auto res = ebnf::parse("rule0 ::= ('a')? ('b') ('c')?");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 3)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[1].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[1].children[0].content == "b")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[2].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[2].children[0].content == "c")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })

        qb_test("rule0 ::= ('a')*", {
            auto res = ebnf::parse("rule0 ::= ('a')*");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 1)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })

        qb_test("rule0 ::= ('a')* ('b') ('c')*", {
            auto res = ebnf::parse("rule0 ::= ('a')* ('b') ('c')*");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 3)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[1].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[1].children[0].content == "b")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[2].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[2].children[0].content == "c")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })

        qb_test("rule0 ::= ('a')+", {
            auto res = ebnf::parse("rule0 ::= ('a')+");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 1)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })

        qb_test("rule0 ::= ('a')+ ('b') ('c')+", {
            auto res = ebnf::parse("rule0 ::= ('a')+ ('b') ('c')+");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 3)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "a")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[1].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[1].children[0].content == "b")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[2].children[0].type == ebnf::Term::Type::LITERAL)
            qb_assert(res.rules->at("rule0").terms[2].children[0].content == "c")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
    })

    qb_describe("Modifier (Group + Identifier)", {
        
        qb_test("rule0 ::= (rule1)", {
            auto res = ebnf::parse("rule0 ::= (rule1)");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 1)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::NONE)
            delete res.rules;
        })
        
        qb_test("rule0 ::= (rule1)?", {
            auto res = ebnf::parse("rule0 ::= (rule1)?");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 1)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })

        qb_test("rule0 ::= (rule1)? (rule2) (rule3)?", {
            auto res = ebnf::parse("rule0 ::= (rule1)? (rule2) (rule3)?");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 3)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::OPTIONAL)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[1].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[1].children[0].content == "rule2")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[2].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[2].children[0].content == "rule3")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::OPTIONAL)
            delete res.rules;
        })
        
        qb_test("rule0 ::= (rule1)*", {
            auto res = ebnf::parse("rule0 ::= (rule1)*");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 1)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })

        qb_test("rule0 ::= (rule1)* (rule2) (rule3)*", {
            auto res = ebnf::parse("rule0 ::= (rule1)* (rule2) (rule3)*");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 3)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[1].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[1].children[0].content == "rule2")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[2].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[2].children[0].content == "rule3")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::ZERO_OR_N)
            delete res.rules;
        })
        
        qb_test("rule0 ::= (rule1)+", {
            auto res = ebnf::parse("rule0 ::= (rule1)+");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 1)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })

        qb_test("rule0 ::= (rule1)+ (rule2) (rule3)+", {
            auto res = ebnf::parse("rule0 ::= (rule1)+ (rule2) (rule3)+");
            ASSERT_RULES("rule0")
            qb_assert(res.rules->at("rule0").terms.size() == 3)
            qb_assert(res.rules->at("rule0").terms[0].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[0].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[0].children[0].content == "rule1")
            qb_assert(res.rules->at("rule0").terms[0].modifier == ebnf::Term::Modifier::ONE_OR_N)
            qb_assert(res.rules->at("rule0").terms[1].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[1].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[1].children[0].content == "rule2")
            qb_assert(res.rules->at("rule0").terms[1].modifier == ebnf::Term::Modifier::NONE)
            qb_assert(res.rules->at("rule0").terms[2].type == ebnf::Term::Type::GROUP)
            qb_assert(res.rules->at("rule0").terms[2].children[0].type == ebnf::Term::Type::IDENTIFIER)
            qb_assert(res.rules->at("rule0").terms[2].children[0].content == "rule3")
            qb_assert(res.rules->at("rule0").terms[2].modifier == ebnf::Term::Modifier::ONE_OR_N)
            delete res.rules;
        })
    })
})