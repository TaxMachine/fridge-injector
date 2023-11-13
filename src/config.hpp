//
// Created by taxis on 2023-11-12.
//

#ifndef FRIDGE_INJECTOR_CONFIG_HPP
#define FRIDGE_INJECTOR_CONFIG_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <map>

class Config {
    public:
        explicit Config(const std::string& path) {
            this->m_path = path;
            this->load();
        }

        void load() {
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

        void save() {
            std::ofstream file(this->m_path);
            for (auto& [key, value] : this->m_config) {
                file << key << " " << value << std::endl;
            }
        }

        std::string get(const std::string& key) {
            return this->m_config[key];
        }

        void set(const std::string& key, const std::string& value) {
            this->m_config[key] = value;
            this->save();
        }

    private:
        std::string m_path;
        std::map<std::string, std::string> m_config;
};

#endif //FRIDGE_INJECTOR_CONFIG_HPP
