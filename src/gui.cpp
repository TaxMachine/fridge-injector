//
// Created by TaxMachine on 2023-11-08.
//

#include "gui.hpp"

#include <thread>

#include "imgui.h"
#include "injection.hpp"
#include "exceptions.hpp"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"

#ifdef WIN32
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <signal.h>
#endif


static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static std::string openFileDialog(const char* filetypes) {
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filetypes;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileNameA(&ofn);
    return ofn.lpstrFile;
}

GUI::GUI() {
    this->m_version = "1.20.2";
    this->m_dllPath = this->m_config.get("dllPath");
    this->m_minecraftVersions = Injection::getMinecraftVersions();
    glfwInit();
}

void GUI::renderMain() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    ImGui::SetWindowSize(ImVec2((float)width, ((float)height - this->m_titlebarHeight)), ImGuiCond_Always | WS_OVERLAPPEDWINDOW);
    ImGui::SetWindowPos(ImVec2(0, this->m_titlebarHeight), ImGuiCond_Always);
    ImGui::SetNextWindowContentSize(ImVec2((float)width, (float)height));
    ImGui::SetWindowFontScale(2.0f);
    ImGui::SetNextWindowSizeConstraints(ImVec2((float)width, (float)height), ImVec2((float)width, (float)height));
    this->renderTitleBar();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));

    ImGui::BeginGroup();
    ImGui::Text("DLL injector for Minecraft");
    ImGui::Text("Made by TaxMachine");
    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::BeginGroup();
    if (ImGui::Button("Select DLL")) {
        this->m_dllPath = openFileDialog("DLL Files (*.dll)\0*.dll\0");
        if (this->m_dllPath.empty())
            this->m_dllPath = "No DLL selected";
        if (this->m_config.get("dllPath") != this->m_dllPath)
            this->m_config.set("dllPath", this->m_dllPath);
    }
    ImGui::SameLine();
    ImGui::Text("%s", this->m_dllPath.c_str());
    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::Text("Minecraft version");
    ImGui::BeginGroup();
    if (ImGui::BeginCombo("##version", this->m_version.c_str())) {

        if (m_minecraftVersions.empty()) {
            ImGui::Text("No Minecraft windows found");
        }
        for (const std::string &version: m_minecraftVersions) {
            if (ImGui::Selectable(version.c_str())) {
                this->m_version = version;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::BeginGroup();
    if (ImGui::Button("Inject")) {
        if (this->m_dllPath.empty()) {
            ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("No DLL selected");
            ImGui::EndPopup();
        } else {
            try {
                ImGui::BeginPopupModal("Injecting", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Injecting...");
                ImGui::EndPopup();
                //Injection::inject(this->m_dllPath, "Minecraft " + this->m_version);
            } catch (InjectionException& e) {
                ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0));
                ImGui::Text("%s", e.what());
                ImGui::PopStyleColor();
                ImGui::EndPopup();
            } catch (NotImplementedException& e) {
                ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0));
                ImGui::Text("%s", e.what());
                ImGui::PopStyleColor();
                ImGui::EndPopup();
            }
        }
    }
    ImGui::EndGroup();
    ImGui::PopStyleVar();
}

void GUI::render() {
    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    if (!glfwInit())
        return;
    m_window = glfwCreateWindow(700, 600, "fridge injector", nullptr, nullptr);
    if (m_window == nullptr)
        return;
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        this->renderMain();
        ImGui::Render();

        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window, true);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
        int app_width, app_height;
        glfwGetFramebufferSize(m_window, &app_width, &app_height);
        glViewport(0, 0, app_width, app_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void GUI::renderTitleBar() {
    ImGui::BeginMainMenuBar();
    this->m_titlebarHeight = ImGui::GetFrameHeight();
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open")) {
            this->m_dllPath = openFileDialog("DLL Files (*.dll)\0*.dll\0");
        }
        if (ImGui::MenuItem("Exit")) {
            glfwSetWindowShouldClose(m_window, true);
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

