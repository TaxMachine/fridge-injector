//
// Created by TaxMachine on 2023-11-08.
//

#ifndef FRIDGE_INJECTOR_GUI_HPP
#define FRIDGE_INJECTOR_GUI_HPP

#include <string>
#include <vector>

#include "GLFW/glfw3.h"
#include "config.hpp"

class GUI {
    public:
        GUI();
        void render();
        std::string m_version;
        std::string m_dllPath;
        Config m_config = Config("config.txt");
    private:
        void renderMain();
        void renderTitleBar();
        GLFWwindow* m_window{};
        std::vector<std::string> m_minecraftVersions;

        float m_titlebarHeight = 0;
};

#endif //FRIDGE_INJECTOR_GUI_HPP
