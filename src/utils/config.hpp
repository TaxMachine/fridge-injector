//
// Created by taxis on 2023-11-12.
//

#ifndef FRIDGE_INJECTOR_CONFIG_HPP
#define FRIDGE_INJECTOR_CONFIG_HPP

#include <string>
#include <map>

class Config {
    public:
        explicit Config(const std::string& path);
        void load();
        void save();
        void set(const std::string& key, const std::string& value);
        std::string get(const std::string& key);

    private:
        std::string m_path;
        std::map<std::string, std::string> m_config;
};

#endif //FRIDGE_INJECTOR_CONFIG_HPP
