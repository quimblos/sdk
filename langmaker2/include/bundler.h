#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include "syntax/writer.h"
#include "semantics/writer.h"

void bundle(const std::string& langname, const syntax::writer::res_t& syntax, const semantics::writer::res_t& semantics, const std::string& include_path, const std::string& src_path);