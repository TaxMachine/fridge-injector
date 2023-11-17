//
// Created by taxis on 2023-11-14.
//

#include <ranges>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#include "exceptions.hpp"
#include "utils.hpp"

#ifdef WIN32
#include <Windows.h>
#elif __linux__
#include <string.h>
#endif

std::string Utils::openFileDialog(const char* filters) {
#ifdef WIN32
    OPENFILENAME ofn;
    char szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filters;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileName(&ofn);
    return std::string{ofn.lpstrFile};
#elif __linux__
    char path[1024] = {0};
    FILE* fp = popen("zenity --file-selection --file-filter=*.so --title \"Select your cheat shared library\"", "r");
    fgets(path, sizeof(path), fp);
    char* newline = strchr(path, '\n');
    if (newline != nullptr)
        *newline = 0;
    pclose(fp);
    return {path};

#endif
}

std::vector<std::string> Utils::split(std::string str, const std::string& delim) {
    std::vector<std::string> split;
    size_t pos;
    std::string token{};
    while ((pos = str.find(delim)) != std::string::npos) {
        token = str.substr(0, pos);
        split.push_back(token);
        str.erase(0, pos + delim.length());
    }
    split.push_back(str);
    return split;
}

unsigned long Utils::getFileSize(const std::string& path) {
    if (!std::filesystem::exists(path))
        throw std::invalid_argument("File not found");

    std::ifstream f(path);
    f.seekg(0, std::ios::end);
    const unsigned long& size = f.tellg();
    f.close();
    return size;
}