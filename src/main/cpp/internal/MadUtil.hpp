//
// Created by William Kamp on 10/15/16.
//

#include <string>

namespace madsqlite {

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

    static std::string getAbsoluteFilePath(std::string const &filePath) {
        std::string absolutePath;
#ifdef _WIN32
        char absolute[_MAX_PATH];
        if (_fullpath(absolute, filePath.c_str(), _MAX_PATH))
            absolute_path = absolute;
#elif defined(__linux__) || defined(__sun) || defined(__hpux) || defined(__GNUC__)
        char *absolute = realpath(filePath.c_str(), nullptr);
        if (absolute)
            absolutePath = absolute;
        free(absolute);
#else
#error Platform absolute path function needed
#endif
        return absolutePath;
    }

}