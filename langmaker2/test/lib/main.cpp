#include <iostream>
#include "test.h"
#include "../ebnf.test.h"
#include "../parser.test.h"
#include "../semantics.test.h"
#include "../meta.test.h"
#include "../semantics_syntax.test.h"
#include "../parser_semantics.test.h"

int main(int argc, char* argv[]) {
    
    test_ebnf();
    test_parser();
    test_semantics();
    test_parser_semantics();
    test_semantics_syntax();
    test_meta();
    
    report_test_results();
    return 0;
}
