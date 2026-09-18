#include <string>

const std::string pad(const std::string& input, const std::string& pad = "  ", const std::string& pad0 = "") {
    auto str = std::string(input);
    size_t i = 0;
    while (i < str.size()-1) {
        if (str[i] == '\n') {
            str.replace(i+1, 0, pad);
            i += pad.length() + 1;
        }
        else i++;
    }
    return pad0 + str;
}