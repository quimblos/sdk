#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "semantics/parser.h"
#include "semantics/writer.h"
#include "util/string.h"

// header

std::string write_fwd_declarations(const std::string& langname, const semantics::Schema& schema) {
    std::ostringstream ss;
    for (const auto& rule: schema.rules) {
        ss << "struct " << rule.ast << ";" << std::endl;
    }
    // ss << "  namespace print {" << std::endl;
    // for (const auto& rule: schema.rules) {
    //     ss << "    const std::string " << rule.ast << "(const " << langname << "::" << rule.ast << "& node);";
    //     ss << std::endl;
    // }
    // ss << "  }" << std::endl;
    return ss.str();
}

std::string write_header_prop(const semantics::Schema& schema, const semantics::Prop& prop, const semantics::writer::Config& config) {
    std::ostringstream ss;

    if (prop.modifiers.extract_text) {
        if (prop.modifiers.array_of) {
            ss << "std::vector<std::string> " << prop.key << " = {};";
        }
        else {
            ss << "std::string " << prop.key << " = \"\";";
        }
    }
    else {
        if (prop.modifiers.array_of) {
            ss << "std::vector<" << prop.rule->ast << "*> " << prop.key << " = {};";
        }
        else {
            ss << prop.rule->ast << "* " << prop.key << " = nullptr;";
        }
    }
    ss << std::endl;
    return ss.str();
}

std::string write_header_rule(const std::string& langname, const semantics::Schema& schema, const semantics::Rule& rule, const semantics::writer::Config& config) {
    std::ostringstream ss;
    ss << "struct " << rule.ast << " : public ASTNode {" << std::endl;
    for (const auto& prop: rule.props) {
        ss << "  " << write_header_prop(schema, prop, config);
    }
    ss << std::endl;
    // ss << "  " << rule.ast << "(CSTNode& cst): ASTNode(cst) {}" << std::endl;
    // ss << "  ~" << rule.ast << "();" << std::endl;
    // ss << "  const std::string to_str() const;" << std::endl;
    ss << "  __" << langname << "_NODE(" << rule.ast << ")" << std::endl;
    ss << "};" << std::endl;

    return ss.str();
}

// cpp

std::string write_cpp_prop_constructor(const std::string& langname, const std::string& var, const semantics::Prop& prop) {
    std::ostringstream ss;

    if (prop.modifiers.extract_text) {
        if (prop.value.size() == 0) {
            ss << "_SMT_PROP_TEXT(" << prop.key << ")";
        }
        else {
            if (prop.modifiers.array_of) {
                // TODO
            }
            else {
                // TODO
            }
        }
    }
    else {
        if (prop.modifiers.array_of) {
            ss << "_SMT_ALL(" << prop.key << "," << (uint16_t) prop.term << "," << langname << "::" << prop.rule->ast << ")";
        }
        else {
            ss << "_SMT_FIRST(" << prop.key << "," << (uint16_t) prop.term << "," << langname << "::" << prop.rule->ast << ")";
        }
    }
    return ss.str();
}

std::string write_cpp_prop_destructor(const std::string& langname, const std::string& var, const semantics::Prop& prop) {
    std::ostringstream ss;

    if (prop.modifiers.extract_text) {
        
    }
    else {
        if (prop.modifiers.array_of) {
            ss << "for (const auto& it: " << prop.key << ") delete it;";
        }
        else {
            ss << "delete " << prop.key << ";";
        }
    }
    return ss.str();
}

std::string write_cpp_prop_print(const std::string& langname, const std::string& var, const semantics::Prop& prop) {
    std::ostringstream ss;

    ss << "ss << \"  " << prop.key << ": ";
    if (prop.modifiers.array_of) {
        if (prop.modifiers.extract_text) {
            ss << "[\";" << std::endl;
            ss << "  for (const auto& it: " << prop.key << ") ss << \"'\" << it << \"'\";" << std::endl;
        }
        else {
            ss << "[\" << std::endl;" << std::endl;
            ss << "  for (const auto& it: " << prop.key << ") ss << \"  \" << pad(it->to_str(), \"    \", \"  \") << \",\" << std::endl;" << std::endl;
        }
        ss << "ss << \"  ]\" << std::endl;" << std::endl;
    }
    else {
        if (prop.modifiers.extract_text) {
            ss << "'\" << " << prop.key << " << \"',\" << std::endl;" << std::endl;
        }
        else {
            ss << "\" ";
            ss << "<< ((" << prop.key << " != nullptr) ? pad(" << prop.key << "->to_str()) : \"\") << \",\" << std::endl;" << std::endl;
        }
    }
    return ss.str();
}

std::string write_cpp_rule_constructor(const std::string& langname, const semantics::Schema& schema, const semantics::Rule& rule) {
    std::ostringstream ss;
    ss << langname << "::" << rule.ast << "::" << rule.ast << "(const CSTNode& cst, const std::string& input): ASTNode(cst) {" << std::endl;
    for (const auto& prop: rule.props) {
        ss << "  " << write_cpp_prop_constructor(langname, "grammar", prop) << std::endl;
    }
    ss << "}" << std::endl;

    return ss.str();
}

std::string write_cpp_rule_destructor(const std::string& langname, const semantics::Schema& schema, const semantics::Rule& rule) {
    std::ostringstream ss;
    ss << langname << "::" << rule.ast << "::~" << rule.ast << "() {" << std::endl;
    for (const auto& prop: rule.props) {
        ss << "  " << write_cpp_prop_destructor(langname, "grammar", prop) << std::endl;
    }
    ss << "}" << std::endl;

    return ss.str();
}

std::string write_cpp_rule_print(const std::string& langname, const semantics::Schema& schema, const semantics::Rule& rule) {
    std::ostringstream ss;
    ss << "const std::string " << langname << "::" << rule.ast << "::to_str() const {" << std::endl;
    ss << "  std::ostringstream ss;" << std::endl;
    ss << "  ss << \"" << rule.ast << "{\" << std::endl;" << std::endl;
    for (const auto& prop: rule.props) {
        ss << pad(write_cpp_prop_print(langname, "grammar", prop), "  ", "  ");
    }
    ss << "  ss << \"}\";" << std::endl;
    ss << "  return ss.str();" << std::endl;
    ss << "}" << std::endl;

    return ss.str();
}

std::string write_cpp_rule(const std::string& langname, const semantics::Schema& schema) {
    std::ostringstream ss;
    for (const auto& rule: schema.rules) {
        ss << write_cpp_rule_constructor(langname, schema, rule);
        ss << write_cpp_rule_destructor(langname, schema, rule);
        ss << write_cpp_rule_print(langname, schema, rule);
        ss << std::endl;
    }
    return ss.str();
}

// build

// std::string write_build_rule(const std::string& var, const semantics::Rule& rule) {
//     std::ostringstream ss;
//     ss << rule.ast << " " << var << " = " << rule.ast << "(_" << rule.cst << ");" << std::endl;
//     for (const auto& prop: rule.props) {
//         ss << "    " << write_build_prop(var, prop, rule.cst) << std::endl;
//     }
//     ss << std::endl;
//     return ss.str();
// }

std::string write_build(const std::string langname, const semantics::Rule& grammar_rule) {
    std::ostringstream ss;
    ss << "const " << langname << "::" << grammar_rule.ast << " " << langname << "::build(std::string input) {" << std::endl;
    ss << "  auto cst = " << langname << "::parse(input);" << std::endl;
    ss << "  auto ast = " << grammar_rule.ast << "(cst, input);" << std::endl;
    ss << "  return ast;" << std::endl;
    ss << "}" << std::endl;
    return ss.str();
}

// *

semantics::writer::res_t semantics::write_parser(const std::string& langname, const syntax::Schema& syntax, std::string def, const semantics::writer::Config& config) {
    
    // Build semantics schema
    auto res = semantics::build(syntax, def);
    if (res.code != semantics::res_t::Code::OK) {
        delete res.schema;
        return {
            .code = semantics::writer::res_t::Code::SEMANTICS_ERROR,
            .semantics_error_code = res.code
        };
    }

    // Assert semantics contains grammar rule
    const Rule* grammar_rule = nullptr;
    for (const auto& rule: res.schema->rules) {
        if (rule.cst == "grammar") grammar_rule = &rule;
    }
    if (grammar_rule == nullptr) {
        delete res.schema;
        return {
            .code = semantics::writer::res_t::Code::NO_GRAMMAR_RULE,
            .semantics_error_code = 0
        };
    }

    // HEADER

    std::ostringstream header;

    header << "#include \"ast.h\"" << std::endl;
    header << std::endl;

    header << "#define __" << langname << "_NODE(NAME) \\" << std::endl;
    header << "    NAME(const CSTNode& cst, const std::string& input); \\" << std::endl;
    header << "    ~NAME(); \\" << std::endl;
    header << "    const std::string to_str() const;" << std::endl;
    header << std::endl;


    header << "namespace " << langname << " {" << std::endl;
    header << std::endl;
    
    // > forward declarations
    header << pad(write_fwd_declarations(langname, *res.schema), "  ", "  ");
    header << std::endl;
    
    // > rules
    for (const auto& rule: res.schema->rules) {
        header << pad(write_header_rule(langname, *res.schema, rule, config), "  ", "  ");
        header << std::endl;
    }

    // > build
    header << "  const " << grammar_rule->ast << " build(std::string input);" << std::endl;

    header << "}" << std::endl;

    // CPP

    std::ostringstream cpp;

    cpp << "#include \"util/string.h\"" << std::endl;
    cpp << "#include \"semantics/impl.h\"" << std::endl;
    cpp << std::endl;

    // > print
    cpp << write_cpp_rule(langname, *res.schema);
    cpp << std::endl;

    // > build
    cpp << write_build(langname, *grammar_rule);
    cpp << std::endl;

    return {
        .code = semantics::writer::res_t::Code::OK,
        .semantics_error_code = 0,
        .schema = res.schema,
        .header = header.str(),
        .cpp = cpp.str()
    };
}