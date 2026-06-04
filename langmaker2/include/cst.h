#pragma once

#include <sstream>
#include <string>
#include <vector>

#define COLOR_CYAN "\033[36m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_PURPLE "\033[35m"
#define COLOR_GRAY "\033[30m"
#define COLOR_RED "\033[31m"
#define COLOR_NC "\033[0m"

struct Error {
    enum Code {
        REQUIRED_TERM = 0x00,
        PARSING_FAILED
    } code;
    uint32_t pos;
    uint16_t pi;
    uint8_t ti;
    std::string message;
};

struct CSTNode {
    enum Kind {
        GRAMMAR = 0x00,
        LITERAL,
        GROUP,
        RULE
    } kind;
    std::string rule;
    
    uint8_t term;
    uint32_t start;
    uint32_t end;
    
    std::vector<CSTNode> children;
    std::vector<Error> errors;

    // For identation-sensitive languages
    uint8_t tab = 0;
    
    std::string text(std::string input) const {
        return input.substr(this->start, this->end-this->start);
    }

    std::string to_str(std::string input, uint16_t depth = 0) const {
        std::ostringstream ss;
        if (this->errors.size()) ss << COLOR_RED << "!" << COLOR_NC;
        else  ss << " ";


        std::string input_format = std::string(this->end-this->start, ' ');
        for (uint16_t i = this->start; i < this->end; i++) {
            if (input[i] == '\n') input_format[i-this->start] = '\\';
            else input_format[i-this->start] = input[i];
        }

        for (uint16_t i = 0; i < depth; i++) {
            ss << " ";
        }
        switch (this->kind) {
            case Kind::GRAMMAR: ss << COLOR_CYAN << "GRAMMAR" << COLOR_NC; break;
            case Kind::LITERAL: ss << COLOR_GRAY << "LITERAL"; break;
            case Kind::GROUP: ss << COLOR_YELLOW << "GROUP" << COLOR_NC; break;
            case Kind::RULE: ss << COLOR_PURPLE << "RULE(" << this->rule << ")"  << COLOR_NC; break;
        }
        ss << " " << +this->term << " [" << this->start << ":" << this->end << "] " << COLOR_GRAY << "'" << input_format << "'" << COLOR_NC << "\n";
        
        for (const auto& error : this->errors) {
            for (uint16_t i = 0; i < depth+1; i++) {
                ss << COLOR_RED << " ";
            }
            ss << "[" << error.pi << "] t" << +error.ti << " : " << error.message << "\n" << COLOR_NC;
        }

        for (const auto& child : this->children) {
            if (child.kind == CSTNode::Kind::LITERAL) continue;
            if (child.rule == "letter") continue;
            if (child.rule == "ws") continue;
            if (child.rule == "eol") continue;
            ss << child.to_str(input, depth+1);
        }
        return ss.str();
    }
};