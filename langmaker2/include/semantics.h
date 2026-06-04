#pragma once

#include <sstream>
#include <string>
#include "ast.h"

namespace semantics {

  struct Prop {
    std::string key;
    std::string value;
    struct {
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
    std::string cst;
    std::string ast;
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
      enum Code {
          OK = 0x00,
          SEMANTICS_SYNTAX_ERROR
      } code;
      const Schema* out = nullptr;
  };

  const CSTNode parse(std::string input);
  const res_t build(const std::string& input);
}
