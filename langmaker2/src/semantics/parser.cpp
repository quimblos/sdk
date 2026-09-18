#include <iostream>
#include "semantics/parser.h"
#include "semantics/impl.h"

/*
    CST Parsers
*/

#define _PARSER_FIRST(NAME, PARENT, T) \
    CSTNode* _##NAME = nullptr; \
    if (PARENT != nullptr) { \
        for (const auto& term : PARENT->children) { \
            if (term.term == T) { _##NAME = (CSTNode*) &term; break; } \
        } \
    }

#define _PARSER_EVERY(NAME, PARENT, T, FN...) \
    for (uint16_t i = 0; i < PARENT->children.size(); i++) { \
        if (PARENT->children[i].term != T) continue; \
        auto _##NAME = &PARENT->children[i]; \
        FN \
    }

#define _PARSER_ALL(NAME, PARENT, FN...) \
    for (uint16_t i = 0; i < PARENT->children.size(); i++) { \
        auto _##NAME = &PARENT->children[i]; \
        FN \
    }

#define _PARSER_FIRST_OF_GROUP(NAME, PARENT, T_GROUP, T) \
    _PARSER_FIRST(group_##NAME, PARENT, T_GROUP) \
    _PARSER_FIRST(NAME, _group_##NAME, T)
    

const semantics::res_t semantics::build(const syntax::Schema& syntax, const std::string& def) {
    auto cst = semantics::parse(def);
    if (cst.errors.size()) {
        return {
            .code = semantics::res_t::Code::SYNTAX_ERROR
        };
    }
    auto schema = new semantics::Schema();

    // std::cout << cst.to_str(def) << std::endl;
    auto root = &cst;
    _PARSER_ALL(decl, root,
        auto _rule = &_decl->children[0];
    
        _PARSER_FIRST(cst, _rule, 0)
        _PARSER_FIRST(ast, _rule, 5)
    
        schema->rules.push_back({
            .cst = _cst->text(def),
            .ast = _ast->text(def),
        });
    
        _PARSER_EVERY(group, _decl, 1,
            _PARSER_FIRST(prop, _group, 1)
            _PARSER_FIRST(key, _prop, 0)
            _PARSER_FIRST(value, _prop, 4)
    
            _PARSER_FIRST_OF_GROUP(extract_text, _value, 0, 0)
            _PARSER_FIRST_OF_GROUP(value_name, _value, 0, 1)
            _PARSER_FIRST_OF_GROUP(array_of, _value, 0, 2)

            std::string value = _value_name == nullptr
                ? ""
                : _value_name->text(def);
            bool extract_text = _value_name == nullptr
                ? true
                : _extract_text != nullptr;
            bool array_of = _array_of != nullptr;

            schema->rules.back().props.push_back({
                .key = _key->text(def),
                .value = value,
                .term = 0,
                .rule = nullptr,
                .modifiers = {
                    .extract_text = extract_text,
                    .array_of = array_of
                }
            });
        )
    )

    for (auto& rule: schema->rules) {
        
        // Check if syntax rule exists
        const syntax::Rule* syntax_rule = nullptr;
        if (syntax.contains(rule.cst)) {
            syntax_rule = &syntax.at(rule.cst);
        }
        if (!syntax_rule) {
            std::cout << "Error: " << "Invalid CST rule '" << rule.cst << "'" << std::endl;
            delete schema;
            return {
                .code = semantics::res_t::Code::INVALID_CST_RULE,
                .schema = nullptr
            };
        }
        
        for (auto& prop: rule.props) {
            if (!prop.modifiers.extract_text) {
                // Check if syntax term exists
                int16_t term = -1;
                for (uint8_t i = 0; i < syntax_rule->terms.size(); i++) {
                    auto& it = syntax_rule->terms[i];
                    if (it.type == syntax::Term::Type::IDENTIFIER && it.content == prop.value) term = i;
                }
                if (term == -1) {
                    std::cout << "Error: " << "Invalid CST term '" << prop.value << "'" << std::endl;
                    delete schema;
                    return {
                        .code = semantics::res_t::Code::INVALID_CST_TERM,
                        .schema = nullptr
                    };
                }
                
                // Check if semantics rule exists
                semantics::Rule* ref = nullptr;
                for (auto& it: schema->rules) {
                    if (it.cst == prop.value) ref = &it;
                }
                if (!ref) {
                    std::cout << "Error: " << "Missing AST rule '" << prop.value << "'" << std::endl;
                    delete schema;
                    return {
                        .code = semantics::res_t::Code::MISSING_RULE,
                        .schema = nullptr
                    };
                }

                prop.term = term;
                prop.rule = ref;
            }
        }
    }
    
    return {
        .code = semantics::res_t::Code::OK,
        .schema = schema
    };
}