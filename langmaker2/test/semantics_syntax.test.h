#pragma once
#include "cst.h"
#include "semantics.h"

qb_suite(test_semantics_syntax, "semantics_syntax", {

    qb_describe("Schema", {
    
        qb_test("rule -> Rule:\n  text = #", {
            std::string input = "rule -> Rule:\n  text = #\n";
            auto res = semantics::build(input);
            qb_assert(res.code == semantics::res_t::Code::OK)
            // std::cout << res.out->to_str() << std::endl;
            qb_assert(res.out->rules.size() == 1)
            qb_assert(res.out->rules[0].cst == "rule")
            qb_assert(res.out->rules[0].ast == "Rule")
            qb_assert(res.out->rules[0].props.size() == 1)
            qb_assert(res.out->rules[0].props[0].key == "text")
            qb_assert(res.out->rules[0].props[0].value == "")
            qb_assert(res.out->rules[0].props[0].modifiers.extract_text == true)
            qb_assert(res.out->rules[0].props[0].modifiers.array_of == false)
            delete res.out;
        })
   
    })
})