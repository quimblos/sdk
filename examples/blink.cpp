#include <iostream>
#include "sdk.h"
#include "os.h"

namespace ledline {

    const uint8_t r_LEDS = 0x00;
    
    class Device : public qb::Device {
        public:    
        Device(): qb::Device("ledline") {
            /* LEDS 0x00 */ this->addRegister(qb::Data::b8());
        }
        
        void set(uint8_t reg_i, qb::Data& value) {
            qb::Device::set(reg_i, value);
            this->print();
        }
        
        void print() {
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
}

std::string blink = HEX({
    /* 0  */ HEADER_QUIMBLOS,
    /* 4  */ qb::CmdCode::USE_DEVICE, 0x07, 'l','e','d','l','i','n','e',
    /* 13 */ qb::CmdCode::SET, 0x00, ledline::r_LEDS, qb::DataType::BITMASK8, 0b01010101,
    /* 18 */ qb::CmdCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
    /* 23 */ qb::CmdCode::SET, 0x00, ledline::r_LEDS, qb::DataType::BITMASK8, 0b10101010,
    /* 28 */ qb::CmdCode::SLEEP, 0x00, 0x00, 0x03, 0xe8,
    /* 33 */ qb::CmdCode::GOTO, 0x00, 0x01
});


os::Runner* runner;
int main(int argc, char* argv[]) {
    
    std::cout << "- Creating Engine..." << std::endl;
    qb::Engine engine;

    std::cout << "- Adding LedLine device..." << std::endl;
    ledline::Device ledline;
    engine.putDevice(ledline);

    std::cout << "- Adding ON constant..." << std::endl;
    engine.putConstant("ON", qb::Data::b8(0b11111111));

    std::cout << "- Parsing hexcode: " << blink << std::endl;
    auto parser_out = qb::parser::parse(engine, "blink", blink);
    if (!parser_out.ok) {
        std::cout << "[error] " << parser_out.message << std::endl;
    }

    std::cout << "- Creating blink runner... " << std::endl;
    auto runner_out = engine.makeRunner<os::Runner>("blink", parser_out.script);
    if (!runner_out.ok) {
        std::cout << "[error] " << runner_out.message << std::endl;
    }

    std::cout << "- Running... " << std::endl;
    runner = runner_out.runner;
    signal(SIGINT, [](int signum) { runner->sigint(); });
    runner->run();
    
    return 0;
}