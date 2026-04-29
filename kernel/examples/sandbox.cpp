#include <iostream>
#include "example.h"
#include "0-blink.h"
#include "1-clock.h"
#include "2-accumulator.h"

int main(int argc, char* argv[]) {
    
    bool print = true;
    while (true) {
        if (print) {
            std::cout << "\n[EXAMPLES]" << std::endl;
            std::cout << "0: Blink" << std::endl;
            std::cout << "1: Clock" << std::endl;
            std::cout << "2: Accumulator" << std::endl;
            std::cout << "X: EXIT" << std::endl;
        }

        char opt = std::cin.get();
        if (opt == 10) {
            print = false;
            continue;
        }
        print = true;
        
        std::cout << "opt: " << +opt << std::endl;

        if (opt == '0') invoke("blink", sandbox::blink);
        else if (opt == '1') invoke("clock", sandbox::clock);
        else if (opt == '2') invoke("accumulator", sandbox::accumulator);
        else break;

        std::cin.clear();
    }
}
