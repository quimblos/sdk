#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "bundler.h"
#include "syntax/writer.h"
#include "semantics/writer.h"

#ifdef WIN32
#define OS_SEP '\\'
#else
#define OS_SEP '/'
#endif

void print_syntax() {
    std::cout << "Syntax: langmaker LANGUAGE_NAME SYNTAX_DEF_PATH SEMANTICS_DEF_PATH" << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cout << "ERROR: Missing required arguments." << std::endl;
        print_syntax();
        return -1;
    }

    std::string langname = argv[1];
    std::cout << "Building language '" << langname << "'" << std::endl;
    
    // EBNF

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
    
    // GSF

    std::string gsf_path = argv[3];
    std::cout << " - Reading GSF file from '" << gsf_path << "'" << std::endl;

    std::ifstream gsf_file(argv[3]);
    std::ostringstream gsf_file_str;
    gsf_file_str << gsf_file.rdbuf();
    std::string gsf = gsf_file_str.str();
    gsf_file.close();
    std::cout << " - GSF file has " << gsf.size() << " bytes" << std::endl;
    
    if (gsf.size() == 0) {
        std::cout << "ERROR: Empty or Not Found GSF file." << std::endl;
        return 1;
    }

    // Syntax

    std::cout << " - Generating syntax parser from EBNF" << std::endl;
    auto syntax = syntax::write_parser(langname, ebnf);

    if (syntax.code != syntax::writer::res_t::Code::OK) {
        std::cout << "ERROR: Failed parsing EBNF." << std::endl;
        std::cout << " - Parser error code:" << +syntax.code << std::endl;
        std::cout << " - EBNF error code:" << +syntax.syntax_error_code << std::endl;
        return -1;
    }
    
    // Semantics

    std::cout << " - Generating semantics parser from GSF" << std::endl;
    auto semantics = semantics::write_parser(langname, *syntax.schema, gsf);

    if (semantics.code != semantics::writer::res_t::Code::OK) {
        std::cout << "ERROR: Failed parsing GSF." << std::endl;
        std::cout << " - Parser error code:" << +semantics.code << std::endl;
        std::cout << " - GSF error code:" << +semantics.semantics_error_code << std::endl;
        return -1;
    }
    
    delete syntax.schema;
    delete semantics.schema;

    // Project structure

    std::filesystem::create_directories(langname);
    
    std::ostringstream include_folder;
    include_folder << langname << OS_SEP << "include" << OS_SEP;
    std::filesystem::create_directories(include_folder.str());

    std::ostringstream src_folder;
    src_folder << langname << OS_SEP << "src" << OS_SEP;
    std::filesystem::create_directories(src_folder.str());
    
    // Project Bundle (Header and CPP)

    bundle(langname, syntax, semantics, include_folder.str(), src_folder.str());

    // Project CLI
    
    std::ostringstream cli_path;
    cli_path << langname << OS_SEP << "src" << OS_SEP << "cli.cpp";
    std::cout << " - Writing language CLI at '" << cli_path.str() << "'" << std::endl;
    std::ofstream cli_cpp_file(cli_path.str());

    cli_cpp_file << "#include <iostream>\n";
    cli_cpp_file << "#include \"" << langname << ".h\"\n";
    cli_cpp_file << "\n";
    cli_cpp_file << "int main(int argc, char* argv[]) {\n";
    cli_cpp_file << "  auto ast = " << langname << "::build(argv[1]); \n";
    cli_cpp_file << "  std::cout << ast.to_str() << std::endl;\n";
    cli_cpp_file << "  return 0;\n";
    cli_cpp_file << "}\n";
    cli_cpp_file.close();

    // CMake
    
    std::ostringstream cmake_path;
    cmake_path << langname << OS_SEP << "CMakeLists.txt";
    std::cout << " - Writing CMake file at '" << cmake_path.str() << "'" << std::endl;

    std::ofstream cmake_file(cmake_path.str());

    cmake_file << "cmake_minimum_required(VERSION 3.22.1)\n";
    cmake_file << "set (CMAKE_CXX_STANDARD 20)\n";
    cmake_file << "set (CMAKE_CXX_STANDARD_REQUIRED ON)\n";
    cmake_file << "set (CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS}\")\n";
    cmake_file << "\n";
    cmake_file << "set(CMAKE_BUILD_TYPE Release)\n";
    cmake_file << "project (" << langname << ")\n";
    cmake_file << "\n";
    cmake_file << "# Static Lib (l" << langname << ".a)\n";
    cmake_file << "\n";
    cmake_file << "set(SOURCES\n";
    cmake_file << "    src/" << langname << ".cpp\n";
    cmake_file << ")\n";
    cmake_file << "set(CLI_SOURCES\n";
    cmake_file << "    src/cli.cpp\n";
    cmake_file << ")\n";
    cmake_file << "\n";
    cmake_file << "add_library(" << langname << " STATIC ${SOURCES})\n";
    cmake_file << "\n";
    cmake_file << "target_include_directories(" << langname << "\n";
    cmake_file << "    PRIVATE ${PROJECT_SOURCE_DIR}/include\n";
    cmake_file << ")\n";
    cmake_file << "\n";
    cmake_file << "# CLI\n";
    cmake_file << "\n";
    cmake_file << "add_executable(" << langname << "-cli ${CLI_SOURCES})\n";
    cmake_file << "\n";
    cmake_file << "target_include_directories(" << langname << "-cli\n";
    cmake_file << "    PRIVATE ${PROJECT_SOURCE_DIR}/include\n";
    cmake_file << ")\n";
    cmake_file << "\n";
    cmake_file << "target_link_libraries(" << langname << "-cli\n";
    cmake_file << "    PUBLIC " << langname << "\n";
    cmake_file << ")\n";

    cmake_file.close();

    // Build Script
    
    std::ostringstream build_sh_path;
    build_sh_path << langname << OS_SEP << "build.sh";
    std::cout << " - Writing build script at '" << build_sh_path.str() << "'" << std::endl;
    std::ofstream build_sh_file(build_sh_path.str());

    build_sh_file << "rm -rf build\n";
    build_sh_file << "mkdir build\n";
    build_sh_file << "cd build\n";
    build_sh_file << "cmake ..\n";
    build_sh_file << "make\n";

    build_sh_file.close();

}