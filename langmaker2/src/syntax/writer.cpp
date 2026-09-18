#include <sstream>
#include "syntax/writer.h"

std::string write_term_name(const syntax::Term& term, uint8_t t) {
    std::ostringstream ss;
    switch (term.type) {
        case syntax::Term::Type::LITERAL: ss << "'" << term.content << "'"; break;
        case syntax::Term::Type::CHARMAP: {
            ss << "[";
            for (auto pair : term.charmap) {
                if (pair.first == pair.second) ss << "\\\\" << +pair.first;
                else ss << "\\\\" << +pair.first << "-\\\\" << +pair.second;
            }
            ss << "]";
            break;
        }
        case syntax::Term::Type::IDENTIFIER: ss << term.content; break;
        case syntax::Term::Type::GROUP: ss << "Group"; break;
    }
    return ss.str();
}

std::string write_on_error(const syntax::writer::Config& config, const std::string& kind, const std::string& rule, const syntax::Term& term, uint8_t t) {
    std::ostringstream ss;
    switch (config.on_error) {
        case syntax::writer::Config::OnError::FAIL: ss << "    _STX___ON_ERROR_FAIL," << std::endl; break;
        case syntax::writer::Config::OnError::STOP:
        case syntax::writer::Config::OnError::CONTINUE: {
            auto term_name = write_term_name(term, t);
            ss << "    _STX__ON_ERROR_NONFAIL(" << kind << ",";
            switch (term.modifier) {
                case syntax::Term::Modifier::NONE: ss << "_STX__ON_ERROR_MOD_REQUIRED"; break;
                case syntax::Term::Modifier::OPTIONAL: ss << "_STX__ON_ERROR_MOD_OPTIONAL"; break;
                case syntax::Term::Modifier::ZERO_OR_N: ss << "_STX__ON_ERROR_MOD_OPTIONAL"; break;
                case syntax::Term::Modifier::ONE_OR_N: ss << "_STX__ON_ERROR_MOD_MANY(" << +t << ", \"" << term_name << "\", " << (rule == "grammar" ? "true" : "false") << ")"; break;
            }
            ss << ", \"" << rule << "\", \"" << term_name << "\")," << std::endl;
            break;
        }
    }
    return ss.str();
}

std::string write_after(const syntax::Term::Modifier modifier) {
    std::ostringstream ss;
    switch (modifier) {
        case syntax::Term::Modifier::NONE: ss << "    _STX__AFTER_REQUIRED," << std::endl; break;
        case syntax::Term::Modifier::OPTIONAL: ss << "    _STX__AFTER_OPTIONAL," << std::endl; break;
        case syntax::Term::Modifier::ZERO_OR_N: ss << "    _STX__AFTER_ZERO_OR_N," << std::endl; break;
        case syntax::Term::Modifier::ONE_OR_N: ss << "    _STX__AFTER_ONE_OR_N," << std::endl; break;
    }
    return ss.str();
}

std::string write_else(const syntax::writer::Config& config, const syntax::Term term, uint8_t t, const std::string& rule) {
    std::ostringstream ss;
    switch (term.modifier) {
        case syntax::Term::Modifier::OPTIONAL: ss << "    _STX__ELSE_OPTIONAL()" << std::endl; break;
        case syntax::Term::Modifier::ZERO_OR_N: ss << "    _STX__ELSE_OPTIONAL()" << std::endl; break;
        case syntax::Term::Modifier::ONE_OR_N: ss << "    _STX__ELSE_MANY(" << +t << ")" << std::endl; break;
    }
    if (
        term.modifier == syntax::Term::Modifier::NONE
     || term.modifier == syntax::Term::Modifier::ONE_OR_N
    ) {
        switch (config.on_error) {
            case syntax::writer::Config::OnError::FAIL: ss << "    _STX__ELSE_REQUIRED_FAIL()"; break;
            case syntax::writer::Config::OnError::STOP: {
                ss << "    _STX__ELSE_REQUIRED_STOP" << "(\"" << write_term_name(term, t) << "\", \"" << rule << "\")" << std::endl; break;
            }
            case syntax::writer::Config::OnError::CONTINUE:
                ss << "    _STX__ELSE_REQUIRED_CONTINUE"; break;
        }
    }
    return ss.str();
}

std::string write_rule(const std::string& name, const std::vector<syntax::Term>& terms, std::string kind, const syntax::writer::Config& config) {
    std::ostringstream ss;
    ss << "_STX_RULE(" << name << "," << kind << ",";

    ss << std::endl;

    bool double_loop = name == "grammar"
        && config.on_error == syntax::writer::Config::OnError::CONTINUE;
    
    if (double_loop) {
        ss << "while(i < n) {" << std::endl;
    }

    for (uint8_t t = 0; t < terms.size(); t++) {
        auto term = terms[t];
        bool next_is_or = t < terms.size()-1 && terms[t+1].type == syntax::Term::Type::OR;
        switch (term.type) {
            case syntax::Term::Type::LITERAL:
                ss << "  if (ti == " << +t << ") _STX_TERM_LITERAL(" << +t << ", \"" << term.content << "\", " << term.content.size() << "," << std::endl;
                if (next_is_or) {
                    ss << "break;" << std::endl;
                    ss << "," << std::endl;
                    ss << "ti += 2; continue;" << std::endl;
                }
                else {
                    ss << write_after(term.modifier);
                    ss << write_else(config, term, t, name);
                }
                ss << "  )" << std::endl;
                break;
            case syntax::Term::Type::CHARMAP:
                ss << "  if (ti == " << +t << ") _STX_TERM_CHARMAP(" << +t << "," << std::endl;   

                ss << "    ";
                for (size_t i = 0; i < term.charmap.size(); i++) {
                    auto pair = term.charmap[i];
                    if (pair.first == pair.second) ss << "ch == " << +pair.first;
                    else ss << "(ch >= " << +pair.first << " && ch <= " << +pair.second << ")";
                    if (i < term.charmap.size()-1) ss << " || ";
                }
                ss << "," << std::endl;
                if (next_is_or) {
                    ss << "break;" << std::endl;
                    ss << "ti += 2; continue;";
                }
                else {
                    ss << write_after(term.modifier);
                    ss << write_else(config, term, t, name);
                }
                ss << "  )" << std::endl;
                break;
            case syntax::Term::Type::IDENTIFIER:
                ss << "  if (ti == " << +t << ") _STX_TERM_RULE(" << term.content << "," << +t << "," << std::endl;
                if (next_is_or) {
                    ss << "_STX__ON_ERROR_OR," << std::endl;
                    ss << "_STX__AFTER_OR," << std::endl;
                    ss << "_STX__ELSE_OR";
                }
                else {
                    ss << write_on_error(config, kind, name, term, t);
                    ss << write_after(term.modifier);
                    ss << write_else(config, term, t, name);
                }
                ss << "  )" << std::endl;
                break;
            case syntax::Term::Type::GROUP:
                ss << "  if (ti == " << +t << ") _STX_TERM_RULE(" << name << "_" << +t << "," << +t << "," << std::endl;
                if (next_is_or) {
                    ss << "_STX__ON_ERROR_OR," << std::endl;
                    ss << "_STX__AFTER_OR," << std::endl;
                    ss << "_STX__ELSE_OR";
                }
                else {
                    ss << write_on_error(config, kind, name, term, t);
                    ss << write_after(term.modifier);
                    ss << write_else(config, term, t, name);
                }
                ss << "  )" << std::endl;
                break;
        }
    }
    if (double_loop) {
        ss << "  else {" << std::endl;
        ss << "    i++;" << std::endl;
        ss << "    if (i < n) ti = 0;" << std::endl;
        ss << "    break;" << std::endl;
        ss << "  }" << std::endl;
        ss << "}" << std::endl;
    }
    else {
        ss << "  else break;" << std::endl;
    }
    ss << "," << std::endl;

    bool has_or_term = false;
    for (uint8_t t = 0; t < terms.size(); t++) {
        auto term = terms[t];
        if (term.type == syntax::Term::Type::OR) {
            has_or_term = true;
            break;
        }
    }
    if (has_or_term) {
        ss << "if (i == start) _STX__ERROR_REQUIRED(\"" << name << "\")" << std::endl;
    }
    else {
        uint8_t tc = 0;
        for (uint8_t t = 0; t < terms.size(); t++) {
            auto term = terms[t];
            if (
                term.modifier != syntax::Term::Modifier::NONE
                && term.modifier != syntax::Term::Modifier::ONE_OR_N
            ) continue;
            if (tc > 0) ss << " else";
            tc++;
            switch (term.type) {
                case syntax::Term::Type::LITERAL:
                    ss << " if (ti <= " << +t << ") _STX__ERROR_REQUIRED(\"" << write_term_name(term, t) << "\")" << std::endl;
                    break;
                case syntax::Term::Type::CHARMAP:
                    ss << " if (ti <= " << +t << ") _STX__ERROR_REQUIRED(\"" << write_term_name(term, t) << "\")" << std::endl;
                    break;
                case syntax::Term::Type::IDENTIFIER:
                    ss << " if (ti <= " << +t << ") _STX__ERROR_REQUIRED(\"" << write_term_name(term, t) << "\")" << std::endl;
                    break;
                case syntax::Term::Type::GROUP:
                    ss << " if (ti <= " << +t << ") _STX__ERROR_REQUIRED(\"" << write_term_name(term, t) << "\")" << std::endl;
                    break;
            }
        }
    }

    ss << ")\n" << std::endl;

    
    for (uint8_t t = 0; t < terms.size(); t++) {
        auto term = terms[t];
        if (term.type == syntax::Term::Type::GROUP) {
            std::ostringstream subname;
            subname << name << "_" << +t;
            ss << write_rule(subname.str(), term.children, "GROUP", config);
        }
    }
    
    return ss.str();
}

std::string write_fwd_declarations(const std::string& name, const std::vector<syntax::Term>& terms) {
    std::ostringstream cpp;
    cpp << "const CSTNode* parse_" << name << "(const std::string& input, uint32_t n, uint32_t i, uint8_t term);" << std::endl;

    for (uint8_t t = 0; t < terms.size(); t++) {
        auto term = terms[t];
        if (term.type == syntax::Term::Type::GROUP) {
            std::ostringstream ss;
            ss << name << "_" << +t;
            cpp << write_fwd_declarations(ss.str(), term.children);
        }
    }

    return cpp.str();
}

syntax::writer::res_t syntax::write_parser(std::string langname, std::string def, const syntax::writer::Config& config) {
    auto res = syntax::parse(def);
    if (res.code != syntax::res_t::Code::OK) {
        delete res.schema;
        return {
            .code = syntax::writer::res_t::Code::SYNTAX_ERROR,
            .syntax_error_code = res.code
        };
    }

    if (!res.schema->contains("grammar")) {
        delete res.schema;
        return {
            .code = syntax::writer::res_t::Code::NO_GRAMMAR_RULE,
            .syntax_error_code = 0
        };
    }

    // HEADER

    std::ostringstream header;

    header << "#include \"cst.h\"" << std::endl;
    header << std::endl;
    header << "namespace " << langname << " {" << std::endl;
    header << "  const CSTNode parse(std::string input);" << std::endl;
    header << "}" << std::endl;

    // CPP

    std::ostringstream cpp;

    if (config.header_path.size()) {
        cpp << "#include \"" << config.header_path << "\"" << std::endl;
    }
    else {
        cpp << "#include \"" << langname << ".h\"" << std::endl;
    }
    cpp << std::endl;

    cpp << "#include \"syntax/impl.h\"" << std::endl;
    
    for (const auto& rule : *res.schema) {
        cpp << write_fwd_declarations(rule.first, rule.second.terms);
    //     cpp << "const CSTNode* parse_" << rule.first << "(const std::string& input, uint32_t n, uint32_t i, uint8_t term);" << std::endl;
    //     for (uint8_t t = 0; t < rule.second.terms.size(); t++) {
    //         auto term = rule.second.terms[t];
    //         if (term.type == syntax::Term::Type::GROUP) {
    //             cpp << "const CSTNode* parse_" << rule.first << "_" << +t << "(const std::string& input, uint32_t n, uint32_t i, uint8_t term);" << std::endl;
    //         }
    //     }
    }   
    cpp << std::endl;

    for (const auto& rule : *res.schema) {
        cpp << write_rule(
            rule.first,
            rule.second.terms,
            rule.first == "grammar" ? "GRAMMAR" : "RULE",
            config
        );
    }   

    cpp << "const CSTNode " << langname << "::parse(std::string def) {" << std::endl;
    cpp << "  auto node_ptr = parse_grammar(def, def.size(), 0, 0);" << std::endl;
    if (config.on_error == syntax::writer::Config::OnError::FAIL) {
        cpp << "  if (node_ptr == nullptr) return {" << std::endl;
        cpp << "    .kind = CSTNode::Kind::GRAMMAR," << std::endl;
        cpp << "    .errors = {{" << std::endl;
        cpp << "      .code = Error::PARSING_FAILED," << std::endl;
        cpp << "      .pos = 0," << std::endl;
        cpp << "      .message = \"Parsing failed\"," << std::endl;
        cpp << "    }}" << std::endl;
        cpp << "  };" << std::endl;
    }
    cpp << "  auto node = *node_ptr;" << std::endl;
    cpp << "  delete node_ptr;" << std::endl;
    cpp << "  return node;" << std::endl;
    cpp << "}" << std::endl;  

    return {
        .code = syntax::writer::res_t::Code::OK,
        .syntax_error_code = 0,
        .schema = res.schema,
        .header = header.str(),
        .cpp = cpp.str()
    };
}