#include <iostream>
#include "sdk.h"

class LedStrip : public qb::Device {
    public:    

    LedStrip(): qb::Device("ledstrip") {
        /* LEDS 0x00 */ this->addRegister(qb::Data::b8());
    }
    
    void update() {
        uint8_t val = this->registers.at(0).as_u8();
        for (uint8_t i = 0; i < 8; i++) {
            std::cout << ((val >> i & 1) ? "\\ | / " : "      ");
        }
        std::cout << std::endl;
        for (uint8_t i = 0; i < 8; i++) {
            std::cout << ((val >> i & 1) ? "- | - " : "      ");
        }
        std::cout << std::endl;
        for (uint8_t i = 0; i < 8; i++) {
            std::cout << ((val >> i & 1) ? "/ | \\ " : "      ");
        }
        std::cout << std::endl;
    }
};