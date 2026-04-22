#include <iostream>
#include "test.h"
#include "../float.test.h"
#include "../data.test.h"
#include "../instruction.test.h"
#include "../operator.test.h"
#include "../program.test.h"
#include "../runner.test.h"
#include "../engine.test.h"

int main(int argc, char* argv[]) {
    
    test_float();
    test_data();
    test_instruction();
    test_operator();
    test_program();
    test_runner();
    test_engine();
    
    report_test_results();
    return 0;
}
