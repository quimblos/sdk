#include <iostream>
#include "test.h"
#include "../type.test.h"
#include "../memory.test.h"
#include "../parser.test.h"
#include "../parser_code.test.h"
#include "../operator_cast.test.h"
#include "../operator.test.h"
#include "../context.test.h"
#include "../stack.test.h"
#include "../thread.test.h"
#include "../driver.test.h"
#include "../node.test.h"
#include "../engine.test.h"

int main(int argc, char* argv[]) {
    
    test_type();
    test_memory();
    test_parser();
    test_parser_code();
    test_operator_cast();
    test_operator();
    test_method();
    test_stack();
    test_thread();
    test_driver();
    test_node();
    test_engine();
    
    report_test_results();
    return 0;
}
