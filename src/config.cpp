//
// Created by taxis on 2023-11-14.
//
#include "config.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

Config::Config(const std::string& path) {
    this->m_path = path;
    this->load();
}

void Config::load() {
    std::ifstream file(this->m_path);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        std::string value;
        if (!(iss >> key >> value)) {
            break;
        }
        this->m_config[key] = value;
    }
}

void Config::save() {
    std::ofstream file(this->m_path);
    for (auto& [key, value] : this->m_config) {
        file << key << " " << value << std::endl;
    }
}

std::string Config::get(const std::string&key) {
    return this->m_config[key];
}

void Config::set(const std::string& key, const std::string& value) {
    this->m_config[key] = value;
    this->save();
}