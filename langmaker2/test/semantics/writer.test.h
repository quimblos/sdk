#pragma once
#include <iostream>
#include <fstream>
#include "util/command.h"
#include "syntax/writer.h"
#include "semantics/writer.h"
#include "bundler.h"


int compile(const std::string& langname, const syntax::writer::res_t& syntax, const semantics::writer::res_t& semantics) {

    bundle(langname, syntax, semantics, "tmp/", "tmp/");
    
    std::ostringstream ss;
    ss << "g++ src/syntax/impl.cpp tmp/" << langname << ".cpp -Iinclude -o tmp/lang";

    auto res = raymii::Command::exec(ss.str());
    std::cout << res.output << std::endl;
    return res.exitstatus;
}

int run(const std::string& input) {

    std::ostringstream ss;
    ss << "tmp/lang \"" << input << "\"";

    auto res = raymii::Command::exec(ss.str());
    std::cout << res.output << std::endl;
    return res.exitstatus;
}

qb_suite(test_semantics_writer, "semantics > writer", {

    qb_describe("Compile", {
    
        qb_test("simple lang", {
            auto langname = "mylang";

            auto syntax = syntax::write_parser(
                langname,
                "grammar ::= animal+\n"
                "animal ::= 'cat' | 'dog'"
            );
            qb_assert_or(syntax.code == 0, {
                delete syntax.schema;
            })
            
            auto semantics = semantics::write_parser(
                langname,
                *syntax.schema,
                "grammar -> Zoo:\n"
                "  animals = animal[]\n"
                "animal -> Animal:\n"
                "  type = #\n"
            );
            qb_assert_or(semantics.code == 0, {
                delete syntax.schema;
                delete semantics.schema;
            })

            delete syntax.schema;
            delete semantics.schema;
            
            auto compile_out = compile(langname, syntax, semantics);
            qb_assert(compile_out == 0)
            
            auto run_out = run("catdog");
            qb_assert(run_out == 0)
        })
        
    })
})