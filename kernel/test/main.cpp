#include "device.h"
#include "runner.h"
#include "test.h"
#include "memory_operators.test.h"
#include "code_parser.test.h"
#include "static_checker.test.h"
#include "runner.test.h"

int main(int argc, char* argv[]) {

    std::cout << "\n" << PURPLE << "[MEMORY OPERATORS]" << NC << std::endl;
    test_memory_operators();

    std::cout << "\n" << PURPLE << "[CODE PARSER]" << NC << std::endl;
    test_code_parser();

    std::cout << "\n" << PURPLE << "[STATIC CHECKER]" << NC << std::endl;
    test_static_checker();

    std::cout << "\n" << PURPLE << "[RUNNER]" << NC << std::endl;
    test_runner();
}