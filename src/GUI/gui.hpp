//
// Created by TaxMachine on 2023-11-08.
//

#ifndef FRIDGE_INJECTOR_GUI_HPP
#define FRIDGE_INJECTOR_GUI_HPP

#include <string>
#include <vector>
#include <filesystem>

#include "GLFW/glfw3.h"
#include "../utils/config.hpp"
#include "../injection/injection.hpp"
#include "../injection/pipeserver.hpp"

class GUI {
    public:
        GUI();
        void render();
        MCInstance m_version{};
        std::string m_dllPath{};
        Config* m_config{};
    private:
        void renderMain();
        GLFWwindow* m_window{};
        std::vector<MCInstance> m_minecraftVersions;
        bool m_alreadySeen = false;
        std::string m_dllhash = "Not Available";
        unsigned long long m_dllsize = 0.0L;
        PipeServer* m_pipeServer = nullptr;
        float m_titlebarHeight = 0;
};

#endif //FRIDGE_INJECTOR_GUI_HPP
