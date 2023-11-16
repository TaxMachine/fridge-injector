//
// Created by taxis on 2023-11-14.
//
#include "utils.hpp"

#include <Windows.h>
#include <ranges>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "exceptions.hpp"

std::filesystem::path Utils::openFileDialog(const char* filters) {
#ifdef WIN32
    OPENFILENAMEA ofn;
    std::string szFile;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile.data();
    ofn.nMaxFile = szFile.size();
    ofn.lpstrFilter = filters;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileNameA(&ofn);
    return std::filesystem::absolute(std::string{ofn.lpstrFile});
#elif __linux__
    std::string filename;
    FILE *f = popen("zenity --file-selection --title \"Select your cheat shared library\"", "r");
    fgets(filename.data(), filename.size(), f);
    pclose(f);
    return std::filesystem::absolute(filename);

#endif
}

std::string Utils::sha1(const std::string& path) {
#ifdef WIN32
        std::string command = "certutil.exe -hashfile " + path + " SHA1";
        FILE* file = _popen(command.c_str(), "r");
#elif __linux__
        std::string command = "sha256sum " + path;
        FILE* file = popen(command.c_str(), "r");
#endif

        if (!file)
            return "Failed to open file";
        std::string buffer;
        std::string output;
        while (fgets(buffer.data(), buffer.size(), file) != nullptr)
            output += buffer;

#ifdef WIN32
        _pclose(file);
        return split(output, "\n")[1];
#elif __linux__
        pclose(file);
        return split(output, " ")[0];
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