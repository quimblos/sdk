#include <iostream>
#include "test.h"
#include "../type.test.h"
#include "../value.test.h"

int main(int argc, char* argv[]) {
    
    test_type();
    test_value();
    
    report_test_results();
    return 0;
}
