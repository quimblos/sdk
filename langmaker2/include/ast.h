#pragma once

#include "cst.h"

struct ASTNode {
  const CSTNode* cst;
  const ASTNode* children;
};
