#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "meta.h"

void print_syntax() {
    std::cout << "Syntax: langmaker LANGUAGE_NAME EBNF_FILE" << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cout << "ERROR: Missing required arguments." << std::endl;
        print_syntax();
        return -1;
    }

    std::string name = argv[1];
    std::cout << "Building language '" << name << "'" << std::endl;
    
    std::string ebnf_path = argv[2];
    std::cout << " - Reading EBNF file from '" << ebnf_path << "'" << std::endl;


    std::ifstream ebnf_file(argv[2]);
    std::ostringstream ebnf_file_str;
    ebnf_file_str << ebnf_file.rdbuf();
    std::string ebnf = ebnf_file_str.str();
    ebnf_file.close();
    std::cout << " - EBNF file has " << ebnf.size() << " bytes" << std::endl;
    
    if (ebnf.size() == 0) {
        std::cout << "ERROR: Empty or Not Found EBNF file." << std::endl;
        return 1;
    }

    std::cout << " - Generating syntax parser from EBNF" << std::endl;
    auto parser = meta::generate_cst_parser(name, ebnf);

    if (parser.code != meta::res_t::Code::OK) {
        std::cout << "ERROR: Failed parsing EBNF." << std::endl;
        std::cout << " - Parser error code:" << +parser.code << std::endl;
        std::cout << " - EBNF error code:" << +parser.ebnf_code << std::endl;
        return -1;
    }

    std::filesystem::create_directories("target/src");
    std::filesystem::create_directories("target/include");

    // syntax.cpp

    std::cout << " - Writing syntax parser at '" << "target/src/syntax.cpp" << "'" << std::endl;
    std::ofstream syntax_cpp_file("target/src/syntax.cpp");
    syntax_cpp_file << parser.cpp;
    syntax_cpp_file.close();

    // [name].h

    std::ostringstream lang_header_name;
    lang_header_name << "target/include/" << name << ".h";
    std::ofstream lang_header_file(lang_header_name.str());
    std::cout << " - Writing language header at '" << lang_header_name.str() << "'" << std::endl;
    
    std::cout << "   - Appending cst header" << std::endl;
    lang_header_file << "/* CST */\n";
    std::ifstream cst_header_file("include/cst.h");
    lang_header_file << cst_header_file.rdbuf();
    cst_header_file.close();

    lang_header_file << '\n';
    lang_header_file << '\n';
    
    std::cout << "   - Appending syntax header" << std::endl;
    lang_header_file << "/* Syntax */\n";
    lang_header_file << parser.header;
    lang_header_file.close();

    // cli.cpp
    
    std::cout << " - Writing compiler CLI at '" << "target/src/cli.cpp" << "'" << std::endl;
    std::ofstream cli_cpp_file("target/src/cli.cpp");

    cli_cpp_file << "#include <iostream>\n";
    cli_cpp_file << "#include \"" << name << ".h\"\n";
    cli_cpp_file << "\n";
    cli_cpp_file << "int main(int argc, char* argv[]) {\n";
    cli_cpp_file << "  auto root = " << name << "::parse(argv[1]); \n";
    cli_cpp_file << "  std::cout << root.to_str(argv[1]) << std::endl;\n";
    cli_cpp_file << "  return 0;\n";
    cli_cpp_file << "}\n";

    cli_cpp_file.close();

    // CMakeLists.txt
    
    std::ofstream cmake_file("target/CMakeLists.txt");

    cmake_file << "cmake_minimum_required(VERSION 3.22.1)\n";
    cmake_file << "set (CMAKE_CXX_STANDARD 20)\n";
    cmake_file << "set (CMAKE_CXX_STANDARD_REQUIRED ON)\n";
    cmake_file << "set (CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS}\")\n";
    cmake_file << "\n";
    cmake_file << "set(CMAKE_BUILD_TYPE Release)\n";
    cmake_file << "project (" << name << ")\n";
    cmake_file << "\n";
    cmake_file << "# Static Lib (l" << name << ".a)\n";
    cmake_file << "\n";
    cmake_file << "set(SOURCES\n";
    cmake_file << "    src/syntax.cpp\n";
    cmake_file << "    ../src/parser.cpp\n";
    cmake_file << ")\n";
    cmake_file << "\n";
    cmake_file << "add_library(" << name << " STATIC ${SOURCES})\n";
    cmake_file << "\n";
    cmake_file << "target_include_directories(" << name << "\n";
    cmake_file << "    PRIVATE ${PROJECT_SOURCE_DIR}/include\n";
    cmake_file << "    PRIVATE ${PROJECT_SOURCE_DIR}/../include\n";
    cmake_file << ")\n";
    cmake_file << "\n";
    cmake_file << "# CLI\n";
    cmake_file << "\n";
    cmake_file << "add_executable(" << name << "-compiler src/cli.cpp)\n";
    cmake_file << "\n";
    cmake_file << "target_include_directories(" << name << "-compiler\n";
    cmake_file << "    PRIVATE ${PROJECT_SOURCE_DIR}/include\n";
    cmake_file << ")\n";
    cmake_file << "\n";
    cmake_file << "target_link_libraries(" << name << "-compiler\n";
    cmake_file << "    PUBLIC " << name << "\n";
    cmake_file << ")\n";

    cmake_file.close();

    // build.sh
    
    std::ofstream build_sh_file("target/build.sh");

    build_sh_file << "rm -rf build\n";
    build_sh_file << "mkdir build\n";
    build_sh_file << "cd build\n";
    build_sh_file << "cmake ..\n";
    build_sh_file << "make\n";

    build_sh_file.close();

}