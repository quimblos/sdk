#include <iostream>
#include "test.h"
#include "../ebnf.test.h"
#include "../parser.test.h"
#include "../meta.test.h"

int main(int argc, char* argv[]) {
    
    test_ebnf();
    test_parser();
    test_meta();
    
    report_test_results();
    return 0;
}
