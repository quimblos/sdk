#include "bundler.h"
#include "inline.h"

void bundle(const std::string& langname, const syntax::writer::res_t& syntax, const semantics::writer::res_t& semantics, const std::string& include_path, const std::string& src_path) {

    // header

    std::ostringstream header_path;
    header_path << include_path << langname << ".h";
    std::ofstream h_file(header_path.str());

    h_file << "/* syntax */" << std::endl;
    h_file << std::endl;

    auto syntax_header = std::string(syntax.header);
    {
        auto&& pos = syntax_header.find("#include \"cst.h\"", size_t{});
        if (pos != std::string::npos) syntax_header.replace(pos, 16, __INLINE_CST);
    }
    h_file << syntax_header;
    h_file << std::endl;
    
    h_file << "/* semantics */" << std::endl;
    h_file << std::endl;

    auto semantics_header = std::string(semantics.header);
    {
        auto&& pos = semantics_header.find("#include \"ast.h\"", size_t{});
        if (pos != std::string::npos) semantics_header.replace(pos, 16, __INLINE_AST);
    }
    {
        auto&& pos = semantics_header.find("#include \"cst.h\"", size_t{});
        if (pos != std::string::npos) semantics_header.replace(pos, 16, "");
    }
    {
        auto&& pos = semantics_header.find("#pragma once", size_t{});
        if (pos != std::string::npos) semantics_header.replace(pos, 12, "");
    }
    h_file << semantics_header;
    h_file << std::endl;
    
    h_file.close();

    // cpp

    std::ostringstream cpp_path;
    cpp_path << src_path << langname << ".cpp";
    std::ofstream cpp_file(cpp_path.str());

    cpp_file << "/* syntax */" << std::endl;
    cpp_file << std::endl;

    auto syntax_cpp = std::string(syntax.cpp);
    {
        auto&& pos = syntax_cpp.find("#include \"syntax/impl.h\"", size_t{});
        if (pos != std::string::npos) syntax_cpp.replace(pos, 24, __INLINE_SYNTAX_IMPL);
    }
    {
        auto&& pos = syntax_cpp.find("#pragma once", size_t{});
        if (pos != std::string::npos) syntax_cpp.replace(pos, 12, "");
    }

    cpp_file << syntax_cpp;
    
    cpp_file
        << "bool syntax::check_literal(const std::string input, uint32_t n, uint32_t i0, const std::string literal) {" << std::endl
        << "  n = n-i0;" << std::endl
        << "  uint32_t ln = literal.size();" << std::endl
        << "  if (ln < n) n = ln;" << std::endl
        << "  uint32_t i = 0;" << std::endl
        << "  for (; i < n; i++) {" << std::endl
        << "      if (input[i+i0] != literal[i]) return false;" << std::endl
        << "  };" << std::endl
        << "  return i == ln;" << std::endl
        << "}" << std::endl;

    cpp_file << std::endl;

    cpp_file << "/* semantics */" << std::endl;
    cpp_file << std::endl;
    
    auto semantics_cpp = std::string(semantics.cpp);
    {
        auto&& pos = semantics_cpp.find("#include \"util/string.h\"", size_t{});
        if (pos != std::string::npos) semantics_cpp.replace(pos, 24, __INLINE_UTIL_STRING);
    }
    {
        auto&& pos = semantics_cpp.find("#pragma once", size_t{});
        if (pos != std::string::npos) semantics_cpp.replace(pos, 12, "");
    }
    {
        auto&& pos = semantics_cpp.find("#include \"semantics/impl.h\"", size_t{});
        if (pos != std::string::npos) semantics_cpp.replace(pos, 27, __INLINE_SEMANTICS_IMPL);
    }
    {
        auto&& pos = semantics_cpp.find("#pragma once", size_t{});
        if (pos != std::string::npos) semantics_cpp.replace(pos, 12, "");
    }
    {
        auto&& pos = semantics_cpp.find("#include \"ast.h\"", size_t{});
        if (pos != std::string::npos) semantics_cpp.replace(pos, 16, "");
    }
    cpp_file << semantics_cpp;
    cpp_file << std::endl;

}