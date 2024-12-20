//
// Created by taxis on 2023-11-13.
//

#ifndef FRIDGE_INJECTOR_UTILS_HPP
#define FRIDGE_INJECTOR_UTILS_HPP

#include <string>
#include <vector>

#ifdef WIN32

#define SEPARATOR "\\"

#else

#define SEPARATOR "/"

#endif

namespace Utils {
    std::vector<std::string> split(std::string str, const std::string& delim);
    std::string openFileDialog(const char* filters);
    unsigned long getFileSize(const std::string& path);
}

#endif //FRIDGE_INJECTOR_UTILS_HPP
