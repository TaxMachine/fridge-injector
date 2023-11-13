//
// Created by TaxMachine on 2023-11-08.
//

#include "gui.hpp"

#include <thread>
#include <filesystem>

#include "imgui.h"
#include "injection.hpp"
#include "exceptions.hpp"
#include "utils.hpp"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"
#include "imgui_internal.h"

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

static void moveBothImGuiAndGLFWWindows(GLFWwindow* window, int x, int y) {
    ImGui::SetWindowPos(ImVec2((float)x, (float)y));
    glfwSetWindowPos(window, x, y);
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

static void ErrorPopup(const char* message) {
    ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                                                                ImGuiWindowFlags_ChildWindow |
                                                                ImGuiWindowFlags_NoDocking);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0));
    ImGui::Text("%s", message);
    if (ImGui::Button("OK")) {
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor();
}

GUI::GUI() {
    this->m_dllPath = this->m_config.get("dllPath");
    this->m_minecraftVersions = Injection::getMinecraftVersions();
    glfwInit();
}


void GUI::renderMain() {
    ImGui::Begin("Fridge Injector", nullptr, ImGuiWindowFlags_NoResize |
                                                                ImGuiWindowFlags_NoCollapse |
                                                                ImGuiWindowFlags_NoMove |
                                                                ImGuiWindowFlags_NoSavedSettings |
                                                                ImGuiWindowFlags_NoBringToFrontOnFocus |
                                                                ImGuiWindowFlags_NoNavFocus |
                                                                ImGuiWindowFlags_NoDocking);
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    ImGui::SetWindowSize(ImVec2((float)width, ((float)height - this->m_titlebarHeight)), ImGuiCond_Always);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowContentSize(ImVec2((float)width, (float)height));
    ImGui::SetWindowFontScale(2.0f);
    ImGui::SetNextWindowSizeConstraints(ImVec2((float)width, (float)height), ImVec2((float)width, (float)height));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 15);

    ImGui::BeginGroup();
    ImGui::Text("DLL injector for Fridge");
    ImGui::Text("Made by TaxMachine");
    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::BeginGroup();

    if (ImGui::Button("Select DLL")) {
        this->m_dllPath = openFileDialog("DLL Files (*.dll)\0*.dll\0");
        if (this->m_dllPath.empty())
            this->m_dllPath = "No DLL selected";
        if (this->m_config.get("dllPath") != this->m_dllPath) {
            this->m_config.set("dllPath", this->m_dllPath);
            this->m_alreadySeen = false;
        }
    }
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(25.0f, 0.0f));
    ImGui::Text("Filename: %s", split(this->m_dllPath, SEPARATOR)[split(this->m_dllPath, SEPARATOR).size() - 1].c_str());
    if (!this->m_dllPath.empty()) {
        if (!this->m_alreadySeen) {
            this->m_dllhash = sha1(this->m_dllPath).c_str();
            this->m_dllsize = std::filesystem::file_size(this->m_dllPath) / 1024;
            this->m_dllclientname = findDllSymbol(this->m_dllPath, "CLIENT").c_str();
            this->m_dllversion = findDllSymbol(this->m_dllPath, "VERSION").c_str();
            this->m_alreadySeen = true;
        }
        ImGui::Text("Sha1: %s", this->m_dllhash);
        ImGui::Text("Size: %f kb", this->m_dllsize);
        ImGui::Text("Client name: %s", this->m_dllclientname);
        ImGui::Text("Client version: %s", this->m_dllversion);
    }
    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::Text("Minecraft version");
    ImGui::BeginGroup();
    if (ImGui::BeginCombo("##version", this->m_version.c_str())) {

        if (this->m_minecraftVersions.empty()) {
            ImGui::Text("No Minecraft windows found");
        }
        for (const std::string &version: this->m_minecraftVersions) {
            if (ImGui::Selectable(version.c_str())) {
                this->m_version = version;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(25.0f, 0.0f));

    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        this->m_minecraftVersions = Injection::getMinecraftVersions();
    }

    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::BeginGroup();
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Inject").x) * 0.5f);
    if (ImGui::Button("Inject")) {
        if (this->m_dllPath.empty()) {
            ErrorPopup("No DLL selected");
        } else if (this->m_version.empty()) {
            ErrorPopup("No Minecraft version selected");
        } else {
            try {
                ImGui::BeginPopupModal("Injecting", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::ProgressBar(0.0f, ImVec2(0.0f, 0.0f));
                int injected;
                while (!injected) {
                    try {
                        //Injection::inject(this->m_dllPath, this->m_version);
                        injected = 1;
                        ImGui::ProgressBar(1.0f, ImVec2(0.0f, 0.0f));
                        if (ImGui::Button("OK")) {
                            ImGui::EndPopup();
                        }
                    } catch (InjectionException& e) {
                        ImGui::Text("%s", e.what());
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }
                }
            } catch (InjectionException& e) {
                ErrorPopup(e.what());
            } catch (NotImplementedException& e) {
                ErrorPopup(e.what());
            }
        }
    }
    ImGui::EndGroup();
    ImGui::PopStyleVar(2);
    ImGui::End();
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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsResizeFromEdges = false;
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

