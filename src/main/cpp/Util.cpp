//
// Created by William Kamp on 10/15/16.
//

#include <string>

static std::string upperCaseString(char const *str) {
    std::string aStr(str);
    for (auto &c: aStr) c = (char) toupper(c);
    return aStr;
}

static std::string lowerCaseString(char const *str) {
    std::string aStr(str);
    for (auto &c: aStr) c = (char) tolower(c);
    return aStr;
}

static std::string upperCaseString(std::string const &str) {
    return upperCaseString(str.c_str());
}

static std::string lowerCaseString(std::string const &str) {
    return lowerCaseString(str.c_str());
}