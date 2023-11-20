//
// Created by taxis on 2023-11-20.
//

#include "console.hpp"

#include <ctime>
#include <thread>

#include "../utils/color.hpp"
#include "imgui.h"
#include "../utils/json.hpp"

Console::Console(PipeServer* pipeServer) {
    this->m_scrollToBottom = false;
    this->m_pipeServer = pipeServer;
}

Console::~Console() = default;

static std::string getTime() {
    const time_t& now = time(nullptr);
    const tm* ltm = localtime(&now);
    return "[" + std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec) + "]";
}

void Console::WaitForLogAndPrint() {
    while (true) {
        Message msg = this->m_pipeServer->read();
        if (!msg.message.empty()) {
            this->write(msg);
        }
        Sleep(1000);
    }
}

void Console::render() {
    ImGui::BeginGroup();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 1));
    //ImGui::PushStyleColor(ImGuiCol_WindowBg, Color(0, 0, 0, 1).toImVec4());
    ImGui::PushStyleColor(ImGuiCol_Border, Color(255, 255, 255, 1).toImVec4());
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Color(255, 255, 255, 1).toImVec4());

    std::thread t(&Console::WaitForLogAndPrint, this);
    t.detach();

    if (this->m_scrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
    }

    this->m_scrollToBottom = false;

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
    ImGui::EndChild();
    ImGui::EndGroup();
}

void Console::write(const Message& msg) {
    this->m_buffer += getTime();
    Color* color;
    switch (msg.type) {
        case LOG:
            color = new Color(255, 255, 255, 1);
            this->m_buffer += " [LOG] ";
            break;
        case SUCCESS:
            color = new Color(0, 255, 0, 1);
            this->m_buffer += " [SUCCESS] ";
            break;
        case WARNING:
            color = new Color(255, 255, 0, 1);
            this->m_buffer += " [WARNING] ";
            break;
        case INFO:
            color = new Color(0, 0, 255, 1);
            this->m_buffer += " [INFO] ";
            break;
        case ERR:
            color = new Color(255, 0, 0, 1);
            this->m_buffer += " [ERROR] ";
            break;
        default:
            color = new Color(255, 0, 0, 1);
            this->m_buffer += " [UNKNOWN] ";
            break;
    }

    this->m_buffer += msg.message;

    ImGui::TextColored(color->toImVec4(), this->m_buffer.c_str());

    this->m_scrollToBottom = true;
}