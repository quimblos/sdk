#pragma once
#include <fstream>
#include "command.h"
#include "meta.h"

int compile(const std::string& langname, const std::string& header, const std::string& cpp) {

    std::ostringstream header_name;
    header_name << "tmp/" << langname << ".h";
    std::ofstream h_file(header_name.str());
    h_file << "#include \"cst.h\"\n";
    h_file << header;
    h_file.close();

    std::ofstream cpp_file("tmp/lang.cpp");
    cpp_file << cpp;
    cpp_file.close();
    
    std::ostringstream ss;
    ss << "g++ -c tmp/lang.cpp -Iinclude";

    auto res = raymii::Command::exec(ss.str());
    std::cout << res.output << std::endl;
    return res.exitstatus;
}

qb_suite(test_meta, "meta", {

    qb_describe("CST Parser", {
    
        qb_test("literal term", {
            auto parser = meta::generate_cst_parser(
                "mylang",
                "grammar ::= 'hello'"
            );
            qb_assert(parser.code == 0)
            auto status = compile("mylang", parser.header, parser.cpp);
            qb_assert(status == 0)
        })
    
        qb_test("charmap term", {
            auto parser = meta::generate_cst_parser(
                "mylang",
                "grammar ::= [abc]"
            );
            qb_assert(parser.code == 0)
            auto status = compile("mylang", parser.header, parser.cpp);
            qb_assert(status == 0)
        })
    
        qb_test("group term", {
            auto parser = meta::generate_cst_parser(
                "mylang",
                "grammar ::= ('a' 'b')"
            );
            qb_assert(parser.code == 0)
            auto status = compile("mylang", parser.header, parser.cpp);
            qb_assert(status == 0)
        })

        qb_test("rule term", {
            auto parser = meta::generate_cst_parser(
                "mylang",
                "greet ::= 'hello'\ngrammar ::= greet"
            );
            qb_assert(parser.code == 0)
            auto status = compile("mylang", parser.header, parser.cpp);
            qb_assert(status == 0)
        })
    })
})