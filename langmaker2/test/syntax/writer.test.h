#pragma once
#include <fstream>
#include "util/command.h"
#include "syntax/writer.h"

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

qb_suite(test_syntax_writer, "syntax > writer", {

    qb_describe("Compile", {
    
        qb_test("literal term", {
            auto syntax = syntax::write_parser(
                "mylang",
                "grammar ::= 'hello'"
            );
            qb_assert_or(syntax.code == 0, {
                delete syntax.schema;
            })
            delete syntax.schema;
            auto status = compile("mylang", syntax.header, syntax.cpp);
            qb_assert(status == 0)
        })
    
        qb_test("charmap term", {
            auto syntax = syntax::write_parser(
                "mylang",
                "grammar ::= [abc]"
            );
            qb_assert_or(syntax.code == 0, {
                delete syntax.schema;
            })
            delete syntax.schema;
            auto status = compile("mylang", syntax.header, syntax.cpp);
            qb_assert(status == 0)
        })
    
        qb_test("group term", {
            auto syntax = syntax::write_parser(
                "mylang",
                "grammar ::= ('a' 'b')"
            );
            qb_assert_or(syntax.code == 0, {
                delete syntax.schema;
            })
            delete syntax.schema;
            auto status = compile("mylang", syntax.header, syntax.cpp);
            qb_assert(status == 0)
        })

        qb_test("or term", {
            auto syntax = syntax::write_parser(
                "mylang",
                "grammar ::= 'a' | 'b'"
            );
            qb_assert_or(syntax.code == 0, {
                delete syntax.schema;
            })
            delete syntax.schema;
            auto status = compile("mylang", syntax.header, syntax.cpp);
            qb_assert(status == 0)
        })

        qb_test("rule term", {
            auto syntax = syntax::write_parser(
                "mylang",
                "greet ::= 'hello'\ngrammar ::= greet"
            );
            qb_assert_or(syntax.code == 0, {
                delete syntax.schema;
            })
            delete syntax.schema;
            auto status = compile("mylang", syntax.header, syntax.cpp);
            qb_assert(status == 0)
        })
    })
})