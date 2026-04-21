#pragma once
#define __OK "\033[32mOK\033[0m"
#define __ERROR "\033[31mERROR\033[0m"

#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define PURPLE "\033[35m"
#define GRAY "\033[30m"
#define NC "\033[0m"

namespace test {
    uint16_t total = 0;
    uint16_t ok = 0;
    uint16_t error = 0;
}

#define qb_suite(FN, NAME, CODE) \
    void FN() { \
        std::cout << std::endl << PURPLE << "[" << NAME << "]" << NC << std::endl; \
        CODE \
    }

#define qb_describe(NAME, CODE) \
    std::cout << "  " << YELLOW << "[" << NAME << "]" << NC << std::endl; \
    CODE

#define qb_test(NAME, CODE...) \
  { \
    bool ok = true; \
    while (true) { \
        CODE \
        break; \
    } \
    if (ok) { \
        test::ok++; \
        std::cout << "    " << __OK << " " << NAME << NC << std::endl; \
    } \
    else { \
        test::error++; \
        std::cout << "    " << __ERROR << " " << NAME << NC << std::endl; \
    } \
    test::total++; \
  }

#define qb_assert(CODE) \
  if (!(CODE)) { \
    ok = false; \
    break; \
  }

#define report_test_results() \
  std::cout << std::endl; \
  std::cout << CYAN << "TOTAL" << NC << " : " << test::total << std::endl; \
  std::cout << __OK << "    : " << test::ok << std::endl; \
  std::cout << __ERROR << " : " << test::error << std::endl; \
  std::cout << std::endl;
