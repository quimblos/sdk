#include <iostream>
#include "test.h"
#include "../syntax/parser.test.h"
#include "../syntax/impl.test.h"
#include "../syntax/writer.test.h"

#include "../semantics/parser.test.h"
#include "../semantics/impl.test.h"
#include "../semantics/writer.test.h"

int main(int argc, char* argv[]) {
    
    test_syntax_parser();
    test_syntax_impl();
    test_syntax_writer();
    
    test_semantics_parser();
    test_semantics_impl();
    test_semantics_writer();
    
    report_test_results();
    return 0;
}
