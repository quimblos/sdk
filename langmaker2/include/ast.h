#pragma once

#include <vector>
#include "cst.h"

struct ASTNode {
  const CSTNode* cst;
  ASTNode(const CSTNode& cst): cst(&cst) {}

  virtual const std::string to_str() const = 0;
};