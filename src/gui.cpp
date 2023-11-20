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
#include "sha1.hpp"
#include "console.hpp"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"
#include "imgui_internal.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef WIN32
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <signal.h>
#endif

#define PIPE_NAME "fridge"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#ifdef __linux__
static void MessageBox(const char* type, const char* message, const char* title) {
    char cmd[1024];
    snprintf(cmd, 1024, R"(zenity --%s --text="%s" --title="%s")", type, message, title);
    FILE* fp = popen(cmd, "r");
    pclose(fp);
}
#endif


static void ErrorPopup(const char* message) {
#ifdef WIN32
    MessageBoxA(nullptr, message, "Error", MB_OK | MB_ICONERROR);
#elif __linux__
    MessageBox("error", message, "Error");
#endif
}

static void runNamedPipe(PipeServer*& pipe_server) {
    try {
        pipe_server = new PipeServer(PIPE_NAME);
    } catch (PipeException& e) {
        ErrorPopup(e.what());
    }
}

GUI::GUI() {
    std::thread t(runNamedPipe, std::ref(this->m_pipeServer));
    t.detach();
    this->m_config = new Config("config.txt");
    this->m_dllPath = this->m_config->get("dllPath");
    this->m_minecraftVersions = Injection::getMinecraftVersions();
    if (this->m_minecraftVersions.empty())
        this->m_version.pid = 0;
    else
        this->m_version = m_minecraftVersions[0];
    glfwInit();
}


void GUI::renderMain() {
    ImGui::Begin("Fridge Injector", nullptr, ImGuiWindowFlags_NoResize |
                                                            ImGuiWindowFlags_NoCollapse |
                                                            ImGuiWindowFlags_NoMove |
                                                            ImGuiWindowFlags_NoSavedSettings |
                                                            ImGuiWindowFlags_NoBringToFrontOnFocus |
                                                            ImGuiWindowFlags_NoNavFocus |
                                                            ImGuiWindowFlags_NoDocking |
                                                            ImGuiWindowFlags_NoSavedSettings);
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    ImGui::SetWindowSize(ImVec2(static_cast<float>(width), (static_cast<float>(height) - this->m_titlebarHeight)), ImGuiCond_Always);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowContentSize(ImVec2(static_cast<float>(width), static_cast<float>(height)));
    ImGui::SetWindowFontScale(2.0f);
    ImGui::SetNextWindowSizeConstraints(ImVec2(static_cast<float>(width), static_cast<float>(height)), ImVec2(static_cast<float>(width), static_cast<float>(height)));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 15);

    ImGui::BeginGroup();
    ImGui::Text("DLL injector for Fridge");
    ImGui::Text("Made by TaxMachine");
    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::BeginGroup();

    if (ImGui::Button("Select DLL")) {
        try {
            const auto& libpath = Utils::openFileDialog("DLL file (*.dll)\0*.dll\0");
            if (libpath.empty() || !(libpath.ends_with(".dll") || libpath.ends_with(".so")))
                ErrorPopup("No shared library selected");
            else {
                this->m_dllPath = libpath;
                this->m_config->set("dllPath", this->m_dllPath);
                this->m_alreadySeen = false;
            }
        } catch (std::length_error& e) {
            ErrorPopup(e.what());
        }
    }
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(25.0f, 0.0f));
    try {
        const std::string& pathdll = !this->m_config->get("dllPath").empty() ? this->m_config->get("dllPath").c_str() : "Not selected";
        const std::vector<std::string>& split = Utils::split(pathdll, SEPARATOR);
        ImGui::Text("Filename: %s", split[split.size() - 1].c_str());
        if (!this->m_dllPath.empty()) {
            if (!this->m_alreadySeen) {
                this->m_dllhash = SHA1::from_file(this->m_dllPath);
                this->m_dllsize = Utils::getFileSize(this->m_dllPath) / 1024;
                this->m_alreadySeen = true;
            }
            std::string b = SHA1::from_file(this->m_dllPath);
            ImGui::Text("Sha1: %s", this->m_dllhash.c_str());
            ImGui::Text("Size: %llu kb", this->m_dllsize);
        }
    } catch (std::length_error& e) {
        ErrorPopup(e.what());
    }
    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::Text("Minecraft version");
    ImGui::BeginGroup();

    std::string preview = this->m_version.pid ? (this->m_version.title + " pid: " + std::to_string(this->m_version.pid)) : "Select a DLL";

    if (ImGui::BeginCombo("##version", preview.c_str())) {
        if (this->m_minecraftVersions.empty()) {
            ImGui::Selectable("No Minecraft windows found");
        } else {
            for (const MCInstance &version: this->m_minecraftVersions) {
                if (ImGui::Selectable((version.title + " pid: " + std::to_string(version.pid)).c_str())) {
                    preview = version.title + " pid: " + std::to_string(version.pid);
                    this->m_version = version;
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(25.0f, 0.0f));

    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        this->m_minecraftVersions = Injection::getMinecraftVersions();
        if (!this->m_minecraftVersions.empty())
            this->m_version = m_minecraftVersions[0];
    }

    ImGui::EndGroup();

    ImGui::Separator();

    ImGui::BeginGroup();
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Inject").x) * 0.5f);
    if (ImGui::Button("Inject")) {
        if (this->m_dllPath.empty()) {
            ErrorPopup("No DLL selected");
        } else if (this->m_version.title.empty() || this->m_version.pid == 0) {
            ErrorPopup("No Minecraft version selected");
        } else {
            try {
                Injection::inject(this->m_dllPath, this->m_version.pid);
#ifdef WIN32
                MessageBox(nullptr, "Injected !", "Success", MB_OK | MB_ICONINFORMATION);
#elif __linux__
                MessageBox("info", "Injected !", "Success");
#endif
            } catch (InjectionException& e) {
                ErrorPopup(e.what());
            } catch (NotImplementedException& e) {
                ErrorPopup(e.what());
            }
        }
    }
#ifdef __linux__
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Uninject").x) * 0.5f);
    if (ImGui::Button("Uninject"))
        try {
            Injection::uninject();
        } catch (InjectionException& e) {
            ErrorPopup(e.what());
        }

#endif
    ImGui::EndGroup();
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Refresh console").x) * 0.5f);
    if (ImGui::Button("Refresh console")) {
        try {
            this->m_pipeServer = new PipeServer(PIPE_NAME);
        } catch (PipeException& e) {
            ErrorPopup(e.what());
        }
    }

    if (this->m_pipeServer != nullptr) {
        Console* console = new Console(this->m_pipeServer);
        console->render();
    }

    ImGui::PopStyleVar(2);
    ImGui::End();
}

void GUI::render() {
    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    if (!glfwInit())
        return;
    m_window = glfwCreateWindow(750, 800, "fridge injector", nullptr, nullptr);
    if (m_window == nullptr)
        return;
    GLFWimage images[1];
    images[0].pixels = stbi_load("assets/fridge.png", &images[0].width, &images[0].height, nullptr, 4);
    glfwSetWindowIcon(m_window, 1, images);
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

