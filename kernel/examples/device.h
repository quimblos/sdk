#include <iostream>
#include "sdk.h"

class LedBar : public qb::Device {
    public:    

    LedBar(): qb::Device("LEDBAR") {
        /* 0x00 */ this->add_node("leds", qb::node::u8());
    }
    
    void update() {
        qb::Node* node = this->nodes.at(0).second;
        uint8_t val = qb::node::as_u8(node);

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