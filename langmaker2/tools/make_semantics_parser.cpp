#include <iostream>
#include <fstream>
#include <sstream>

#include "syntax/writer.h"

int main() {

    std::string path = "schema/semantics.ebnf";

    std::cout << " - Reading EBNF file from " << path << std::endl;
    std::ifstream ebnf_file(path);
    std::ostringstream ebnf_file_str;
    ebnf_file_str << ebnf_file.rdbuf();
    std::string ebnf = ebnf_file_str.str();
    ebnf_file.close();
    std::cout << " - Semantics EBNF file has " << ebnf.size() << " bytes" << std::endl;
    
    if (ebnf.size() == 0) {
        std::cout << "ERROR: Empty or Not Found EBNF file." << std::endl;
        return 1;
    }

    std::cout << " - Generating semantics syntax parser from EBNF" << std::endl;
    auto parser = syntax::write_parser("semantics", ebnf, {
        .on_error = syntax::writer::Config::OnError::STOP,
        .header_path = "semantics/parser.h"
    });
    delete parser.schema;

    if (parser.code != syntax::writer::res_t::Code::OK) {
        std::cout << "ERROR: Failed parsing EBNF." << std::endl;
        std::cout << " - Parser error code:" << +parser.code << std::endl;
        std::cout << " - Syntax error code:" << +parser.syntax_error_code << std::endl;
        return -1;
    }

    // semantics.cpp

    std::cout << " - Writing semantics syntax parser at '" << "src/semantics/impl.cpp" << "'" << std::endl;
    std::ofstream syntax_cpp_file("src/semantics/impl.cpp");
    syntax_cpp_file << parser.cpp;
    syntax_cpp_file.close();

}