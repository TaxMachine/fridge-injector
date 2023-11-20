//
// Created by taxis on 2023-11-20.
//

#include "pipeserver.hpp"

#include "../utils/json.hpp"

PipeServer::PipeServer(const std::string& pipeName) {
    m_pipeName = "\\\\.\\pipe\\" + pipeName;
#ifdef WIN32
    m_pipe = CreateNamedPipeA(
        m_pipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        1024,
        1024,
        0,
        nullptr);
    if (m_pipe == INVALID_HANDLE_VALUE) {
        throw PipeException("Failed to create pipe");
    }
    if (!ConnectNamedPipe(m_pipe, nullptr)) {
        throw PipeException("Failed to connect to pipe");
    }
#else
    throw NotImplementedException("Not implemented");
#endif
}

PipeServer::~PipeServer() {
#ifdef WIN32
    CloseHandle(m_pipe);
#else
    throw NotImplementedException("Not implemented");
#endif
}

void PipeServer::write(const std::string& message) const {
#ifdef WIN32
    DWORD bytesWritten;
    if (WriteFile(m_pipe, message.c_str(), message.size(), &bytesWritten, nullptr)) {
        throw PipeException("Failed to write to pipe");
    }
#else
    throw NotImplementedException("Not implemented");
#endif
}

Message PipeServer::read() const {
#ifdef WIN32
    if (WaitNamedPipeA(m_pipeName.c_str(), 10000)) {
        throw PipeException("Failed to wait for pipe");
    }
    DWORD bytesRead;
    char buffer[1024];
    if (ReadFile(m_pipe, buffer, sizeof(buffer), &bytesRead, nullptr)) {
        throw PipeException("Failed to read pipe");
    }
    try {
        nlohmann::json payload = nlohmann::json::parse(buffer);
        return {
            payload["message"].get<std::string>(),
            static_cast<MessageType>(payload["type"].get<int>()),
            payload["client"].get<std::string>(),
            payload["version"].get<std::string>()};
    } catch (nlohmann::json::exception& e) {
        return {"", static_cast<MessageType>(0), "", ""};
    }
#else
    throw NotImplementedException("Not implemented");
#endif
}