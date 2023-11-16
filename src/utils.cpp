//
// Created by taxis on 2023-11-14.
//
#include "utils.hpp"

#include <Windows.h>
#include <ranges>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#include "exceptions.hpp"

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
    std::string filename;
    FILE *f = popen("zenity --file-selection --title \"Select your cheat shared library\"", "r");
    fgets(filename.data(), filename.size(), f);
    pclose(f);
    return filename;

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