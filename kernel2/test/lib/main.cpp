#include <iostream>
#include "test.h"
#include "../type.test.h"
#include "../memory.test.h"
#include "../parser.test.h"
#include "../operator.test.h"

int main(int argc, char* argv[]) {
    
    test_type();
    test_memory();
    test_parser();
    test_operator();
    
    report_test_results();
    return 0;
}
