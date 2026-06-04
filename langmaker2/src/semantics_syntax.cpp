#include <iostream>
#include "parser_semantics.h"
#include "semantics.h"

const semantics::res_t semantics::build(const std::string& input) {
    auto cst = semantics::parse(input);
    if (cst.errors.size()) {
        return {
            .code = semantics::res_t::Code::SEMANTICS_SYNTAX_ERROR
        };
    }
    auto schema = new semantics::Schema();

    // std::cout << cst.to_str(input) << std::endl;
    auto root = &cst;
    _ALL(decl, root,
        auto _rule = &_decl->children[0];
    
        _FIRST(cst, _rule, 0)
        _FIRST(ast, _rule, 5)
    
        schema->rules.push_back({
            .cst = _cst->text(input),
            .ast = _ast->text(input),
        });
    
        _EVERY(group, _decl, 1,
            _FIRST(prop, _group, 1)
            _FIRST(key, _prop, 0)
            _FIRST(value, _prop, 4)
    
            auto value = _value->text(input);
            bool extract_text = value[0] == '#';
            bool array_of = value.size() >= 2 && value[value.size()-2] == '[' && value[value.size()-1] == ']';
            if (value == "#") value = "";
    
            schema->rules.back().props.push_back({
                .key = _key->text(input),
                .value = value,
                .modifiers = {
                    .extract_text = extract_text,
                    .array_of = array_of
                }
            });
        )
    )
    
    return {
        .code = semantics::res_t::Code::OK,
        .out = schema
    };
}