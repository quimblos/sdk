#include <iostream>
#include <fstream>
#include <sstream>

#ifdef WIN32
#define OS_SEP '\\'
#else
#define OS_SEP '/'
#endif

const std::string read(const std::string& path) {
    std::cout << " - Reading '" << path << "'" << std::endl;

    std::ifstream file(path);
    std::ostringstream buf;
    buf << file.rdbuf();
    std::string str = buf.str();
    file.close();
    std::cout << "   - size: " << str.size() << " bytes" << std::endl;
    
    if (str.size() == 0) {
        std::cout << "   ERROR: File empty or not found." << std::endl;
        return "";
    }

    return str;
}

void replace(
    std::string& str,
    const std::string& from,
    const std::string& to)
{
    auto&& pos = str.find(from, size_t{});
    while (pos != std::string::npos)
    {
        str.replace(pos, from.length(), to);
        pos = str.find(from, pos + to.length());
    }
}

const std::string encode(const std::string& str) {
    auto out = std::string(str);
    replace(out, "\\", "\\\\");
    replace(out, "\"", "\\\"");
    replace(out, "\n", "\\n\" \\\n\"");
    return "\"" + out + "\"";
}

int main() {
    
    std::ostringstream cst_path;
    cst_path << "include" << OS_SEP << "cst.h";
    auto cst = read(cst_path.str());
    
    std::ostringstream ast_path;
    ast_path << "include" << OS_SEP << "ast.h";
    auto ast = read(ast_path.str());
    
    std::ostringstream util_string_path;
    util_string_path << "include" << OS_SEP << "util" << OS_SEP << "string.h";
    auto util_string = read(util_string_path.str());
    
    std::ostringstream syntax_impl_path;
    syntax_impl_path << "include" << OS_SEP << "syntax" << OS_SEP << "impl.h";
    auto syntax_impl = read(syntax_impl_path.str());
    
    std::ostringstream semantics_impl_path;
    semantics_impl_path << "include" << OS_SEP << "semantics" << OS_SEP << "impl.h";
    auto semantics_impl = read(semantics_impl_path.str());

    std::ostringstream inline_path;
    inline_path << "include" << OS_SEP << "inline.h";
    std::cout << " - Writing inline header at '" << inline_path.str() << "'" << std::endl;

    std::ofstream inline_h_file(inline_path.str());
    inline_h_file << "#define __INLINE_CST " << encode(cst) << std::endl;
    inline_h_file << "#define __INLINE_AST " << encode(ast) << std::endl;
    inline_h_file << "#define __INLINE_UTIL_STRING " << encode(util_string) << std::endl;
    inline_h_file << "#define __INLINE_SYNTAX_IMPL " << encode(syntax_impl) << std::endl;
    inline_h_file << "#define __INLINE_SEMANTICS_IMPL " << encode(semantics_impl) << std::endl;
    inline_h_file.close();

}