#include <sstream>
#include "meta.h"

std::string generate_term_name(const ebnf::Term& term, uint8_t t) {
    std::ostringstream ss;
    switch (term.type) {
        case ebnf::Term::Type::LITERAL: ss << "'" << term.content << "'"; break;
        case ebnf::Term::Type::CHARMAP: {
            ss << "[";
            for (auto pair : term.charmap) {
                if (pair.first == pair.second) ss << "\\\\" << +pair.first;
                else ss << "\\\\" << +pair.first << "-\\\\" << +pair.second;
            }
            ss << "]";
            break;
        }
        case ebnf::Term::Type::IDENTIFIER: ss << term.content; break;
        case ebnf::Term::Type::GROUP: ss << "Group"; break;
    }
    return ss.str();
}

std::string generate_on_error(const meta::Config& config, const std::string& kind, const std::string& rule, const ebnf::Term& term, uint8_t t) {
    switch (config.on_error) {
        case meta::Config::OnError::FAIL: return "    __ON_ERROR_FAIL,\n";
        case meta::Config::OnError::STOP:
        case meta::Config::OnError::CONTINUE: {
            auto term_name = generate_term_name(term, t);
            std::ostringstream ss;
            ss << "    __ON_ERROR_NONFAIL(" << kind << ",";
            switch (term.modifier) {
                case ebnf::Term::Modifier::NONE: ss << "__ON_ERROR_MOD_REQUIRED"; break;
                case ebnf::Term::Modifier::OPTIONAL: ss << "__ON_ERROR_MOD_OPTIONAL"; break;
                case ebnf::Term::Modifier::ZERO_OR_N: ss << "__ON_ERROR_MOD_OPTIONAL"; break;
                case ebnf::Term::Modifier::ONE_OR_N: ss << "__ON_ERROR_MOD_MANY(" << +t << ", \"" << term_name << "\", " << (rule == "grammar" ? "true" : "false") << ")"; break;
            }
            ss << ", \"" << rule << "\", \"" << term_name << "\"),\n";
            return ss.str();
        }
    }
}

std::string generate_after(const ebnf::Term::Modifier modifier) {
    switch (modifier) {
        case ebnf::Term::Modifier::NONE: return "    __AFTER_REQUIRED,\n";
        case ebnf::Term::Modifier::OPTIONAL: return "    __AFTER_OPTIONAL,\n";
        case ebnf::Term::Modifier::ZERO_OR_N: return "    __AFTER_ZERO_OR_N,\n";
        case ebnf::Term::Modifier::ONE_OR_N: return "    __AFTER_ONE_OR_N,\n";
    }
}

std::string generate_else(const meta::Config& config, const ebnf::Term term, uint8_t t, const std::string& rule) {
    std::ostringstream ss;
    switch (term.modifier) {
        case ebnf::Term::Modifier::OPTIONAL: ss << "    __ELSE_OPTIONAL()\n"; break;
        case ebnf::Term::Modifier::ZERO_OR_N: ss << "    __ELSE_OPTIONAL()\n"; break;
        case ebnf::Term::Modifier::ONE_OR_N: ss << "    __ELSE_MANY(" << +t << ")\n"; break;
    }
    if (
        term.modifier == ebnf::Term::Modifier::NONE
     || term.modifier == ebnf::Term::Modifier::ONE_OR_N
    ) {
        switch (config.on_error) {
            case meta::Config::OnError::FAIL: ss << "    __ELSE_REQUIRED_FAIL()"; break;
            case meta::Config::OnError::STOP: {
                ss << "    __ELSE_REQUIRED_STOP" << "(\"" << generate_term_name(term, t) << "\", \"" << rule << "\")\n"; break;
            }
            case meta::Config::OnError::CONTINUE:
                ss << "    __ELSE_REQUIRED_CONTINUE"; break;
        }
    }
    return ss.str();
}

std::string generate_rule(const std::string& name, const std::vector<ebnf::Term>& terms, std::string kind, const meta::Config& config) {
    std::ostringstream ss;
    ss << "_RULE(" << name << "," << kind << ",";

    ss << '\n';

    bool double_loop = name == "grammar"
        && config.on_error == meta::Config::OnError::CONTINUE;
    
    if (double_loop) {
        ss << "while(i < n) {\n";
    }

    for (uint8_t t = 0; t < terms.size(); t++) {
        auto term = terms[t];
        bool next_is_or = t < terms.size()-1 && terms[t+1].type == ebnf::Term::Type::OR;
        switch (term.type) {
            case ebnf::Term::Type::LITERAL:
                ss << "  if (ti == " << +t << ") _TERM_LITERAL(" << +t << ", \"" << term.content << "\", " << term.content.size() << ",\n";
                if (next_is_or) {
                    ss << "break;\n";
                    ss << "ti += 2; continue;";
                }
                else {
                    ss << generate_after(term.modifier);
                    ss << generate_else(config, term, t, name);
                }
                ss << "  )\n";
                break;
            case ebnf::Term::Type::CHARMAP:
                ss << "  if (ti == " << +t << ") _TERM_CHARMAP(" << +t << ",\n";   

                ss << "    ";
                for (size_t i = 0; i < term.charmap.size(); i++) {
                    auto pair = term.charmap[i];
                    if (pair.first == pair.second) ss << "ch == " << +pair.first;
                    else ss << "(ch >= " << +pair.first << " && ch <= " << +pair.second << ")";
                    if (i < term.charmap.size()-1) ss << " || ";
                }
                ss << ",\n";
                if (next_is_or) {
                    ss << "break;\n";
                    ss << "ti += 2; continue;";
                }
                else {
                    ss << generate_after(term.modifier);
                    ss << generate_else(config, term, t, name);
                }
                ss << "  )\n";
                break;
            case ebnf::Term::Type::IDENTIFIER:
                ss << "  if (ti == " << +t << ") _TERM_RULE(" << term.content << "," << +t << ",\n";
                if (next_is_or) {
                    ss << "__ON_ERROR_OR,\n";
                    ss << "__AFTER_OR,\n";
                    ss << "__ELSE_OR";
                }
                else {
                    ss << generate_on_error(config, kind, name, term, t);
                    ss << generate_after(term.modifier);
                    ss << generate_else(config, term, t, name);
                }
                ss << "  )\n";
                break;
            case ebnf::Term::Type::GROUP:
                ss << "  if (ti == " << +t << ") _TERM_RULE(" << name << "_" << +t << "," << +t << ",\n";
                if (next_is_or) {
                    ss << "__ON_ERROR_OR,\n";
                    ss << "__AFTER_OR,\n";
                    ss << "__ELSE_OR";
                }
                else {
                    ss << generate_on_error(config, kind, name, term, t);
                    ss << generate_after(term.modifier);
                    ss << generate_else(config, term, t, name);
                }
                ss << "  )\n";
                break;
        }
    }
    if (double_loop) {
        ss << "  else {\n";
        ss << "    i++;\n";
        ss << "    if (i < n) ti = 0;\n";
        ss << "    break;\n";
        ss << "  }\n";
        ss << "}\n";
    }
    else {
        ss << "  else break;\n";
    }
    ss << ",\n";

    bool has_or_term = false;
    for (uint8_t t = 0; t < terms.size(); t++) {
        auto term = terms[t];
        if (term.type == ebnf::Term::Type::OR) {
            has_or_term = true;
            break;
        }
    }
    if (has_or_term) {
        ss << "if (i == start) __ERROR_REQUIRED(\"" << name << "\")\n";
    }
    else {
        uint8_t tc = 0;
        for (uint8_t t = 0; t < terms.size(); t++) {
            auto term = terms[t];
            if (
                term.modifier != ebnf::Term::Modifier::NONE
                && term.modifier != ebnf::Term::Modifier::ONE_OR_N
            ) continue;
            if (tc > 0) ss << " else";
            tc++;
            switch (term.type) {
                case ebnf::Term::Type::LITERAL:
                    ss << " if (ti <= " << +t << ") __ERROR_REQUIRED(\"" << generate_term_name(term, t) << "\")\n";
                    break;
                case ebnf::Term::Type::CHARMAP:
                    ss << " if (ti <= " << +t << ") __ERROR_REQUIRED(\"" << generate_term_name(term, t) << "\")\n";
                    break;
                case ebnf::Term::Type::IDENTIFIER:
                    ss << " if (ti <= " << +t << ") __ERROR_REQUIRED(\"" << generate_term_name(term, t) << "\")\n";
                    break;
                case ebnf::Term::Type::GROUP:
                    ss << " if (ti <= " << +t << ") __ERROR_REQUIRED(\"" << generate_term_name(term, t) << "\")\n";
                    break;
            }
        }
    }

    ss << ")\n\n";

    
    for (uint8_t t = 0; t < terms.size(); t++) {
        auto term = terms[t];
        if (term.type == ebnf::Term::Type::GROUP) {
            std::ostringstream subname;
            subname << name << "_" << +t;
            ss << generate_rule(subname.str(), term.children, "GROUP", config);
        }
    }
    
    return ss.str();
}

std::string generate_fwd_declarations(const std::string& name, const std::vector<ebnf::Term>& terms) {
    std::ostringstream cpp;
    cpp << "const CSTNode* parse_" << name << "(const std::string& input, uint32_t n, uint32_t i, uint8_t term);\n";

    for (uint8_t t = 0; t < terms.size(); t++) {
        auto term = terms[t];
        if (term.type == ebnf::Term::Type::GROUP) {
            std::ostringstream ss;
            ss << name << "_" << +t;
            cpp << generate_fwd_declarations(ss.str(), term.children);
        }
    }

    return cpp.str();
}

meta::res_t meta::generate_cst_parser(std::string langname, std::string ebnf, const meta::Config& config) {
    auto res = ebnf::parse(ebnf);
    if (res.code != ebnf::res_t::Code::OK) {
        delete res.rules;
        return {
            .code = meta::res_t::Code::EBNF_ERROR,
            .ebnf_code = res.code
        };
    }

    if (!res.rules->contains("grammar")) {
        delete res.rules;
        return {
            .code = meta::res_t::Code::NO_GRAMMAR_RULE,
            .ebnf_code = 0
        };
    }

    // HEADER

    std::ostringstream header;

    header << "namespace " << langname << " {\n";
    header << '\n';
    header << "  const CSTNode parse(std::string input);\n";
    header << '\n';
    header << "}\n";

    // CPP

    std::ostringstream cpp;

    cpp << "#include \"parser.h\"\n";
    cpp << "#include \"" << langname << ".h\"\n";
    cpp << '\n';
    
    for (const auto& rule : *res.rules) {
        cpp << generate_fwd_declarations(rule.first, rule.second.terms);
    //     cpp << "const CSTNode* parse_" << rule.first << "(const std::string& input, uint32_t n, uint32_t i, uint8_t term);\n";
    //     for (uint8_t t = 0; t < rule.second.terms.size(); t++) {
    //         auto term = rule.second.terms[t];
    //         if (term.type == ebnf::Term::Type::GROUP) {
    //             cpp << "const CSTNode* parse_" << rule.first << "_" << +t << "(const std::string& input, uint32_t n, uint32_t i, uint8_t term);\n";
    //         }
    //     }
    }   
    cpp << '\n';

    for (const auto& rule : *res.rules) {
        cpp << generate_rule(
            rule.first,
            rule.second.terms,
            rule.first == "grammar" ? "GRAMMAR" : "RULE",
            config
        );
    }   

    cpp << "const CSTNode " << langname << "::parse(std::string input) {\n";
    cpp << "  auto node_ptr = parse_grammar(input, input.size(), 0, 0);\n";
    if (config.on_error == meta::Config::OnError::FAIL) {
        cpp << "  if (node_ptr == nullptr) return {\n";
        cpp << "    .kind = CSTNode::Kind::GRAMMAR,\n";
        cpp << "    .errors = {{\n";
        cpp << "      .code = Error::PARSING_FAILED,\n";
        cpp << "      .pos = 0,\n";
        cpp << "      .message = \"Parsing failed\",\n";
        cpp << "    }}\n";
        cpp << "  };\n";
    }
    cpp << "  auto node = *node_ptr;\n";
    cpp << "  delete node_ptr;\n";
    cpp << "  return node;\n";
    cpp << "}\n";  

    delete res.rules;
    return {
        .code = meta::res_t::Code::OK,
        .ebnf_code = 0,
        .header = header.str(),
        .cpp = cpp.str()
    };
}