//
// Created by taxis on 2023-11-13.
//

#ifndef FRIDGE_INJECTOR_UTILS_HPP
#define FRIDGE_INJECTOR_UTILS_HPP

#include <string>
#include <vector>
#include <filesystem>

#ifdef WIN32

#define SEPARATOR "\\"

#else

#define SEPARATOR "/"

#endif

namespace Utils {
    struct FileFilter {
        std::string name;
        std::vector<std::string> extensions;
        [[nodiscard]] std::string createFilter() const {
            std::string filter = name + " (";
            for (const auto& extension : extensions) {
                filter += "*." + extension + ";";
            }
            filter.pop_back();
            filter += ")";
            return filter;
        }
    };
    std::vector<std::string> split(std::string str, const std::string& delim);
    std::string sha1(const std::string& path);
    std::string findDllSymbol(const std::string& dllPath, const std::string& symbol);
    std::filesystem::path openFileDialog(const std::vector<FileFilter>& filters);
    unsigned long getFileSize(const std::string& path);
}

#endif //FRIDGE_INJECTOR_UTILS_HPP
