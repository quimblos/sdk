#pragma once

#include <sstream>
#include <string>
#include "ast.h"
#include "syntax/parser.h"

namespace semantics {

  struct Rule;
  struct Prop {
    const std::string key;
    const std::string value;
    uint8_t term;
    Rule* rule;
    const struct {
      bool extract_text:1 = false;
      bool array_of:1 = false;
    } modifiers = {};

    std::string to_str() const {
      std::ostringstream ss;
      ss << "  " << key << " = ";
      if (this->modifiers.extract_text) ss << "#"; 
      ss << value;
      if (this->modifiers.array_of) ss << "[]"; 
      return ss.str();
    }
  };
  typedef std::vector<Prop> Props;

  struct Rule {
    const std::string cst;
    const std::string ast;
    Props props;

    std::string to_str() const {
      std::ostringstream ss;
      ss << cst << " -> " << ast << ":\n"; 
      for (const auto& prop : this->props) {
        ss << prop.to_str();
      }
      return ss.str();
    }
  };
  typedef std::vector<Rule> Rules;

  struct Schema {
    Rules rules;

    std::string to_str() const {
      std::ostringstream ss;
      for (const auto& rule : this->rules) {
        ss << rule.to_str();
      }
      return ss.str();
    }
  };
  
  struct res_t {
      const enum Code {
          OK = 0x00,
          SYNTAX_ERROR,
          INVALID_CST_RULE,
          INVALID_CST_TERM,
          MISSING_RULE
      } code;
      const Schema* schema = nullptr;
  };

  const CSTNode parse(std::string def);
  const res_t build(const syntax::Schema& syntax, const std::string& def);
}
