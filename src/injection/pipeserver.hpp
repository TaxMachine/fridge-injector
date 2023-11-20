//
// Created by taxis on 2023-11-20.
//

#ifndef PIPESERVER_HPP
#define PIPESERVER_HPP

#include <string>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

enum MessageType {
    LOG,
    SUCCESS,
    WARNING,
    ERR,
    INFO
};

struct Message {
    std::string message;
    MessageType type;
    std::string client;
    std::string version;
};

class PipeException final : public std::exception {
    public:
        explicit PipeException(const std::string& message) {
            this->m_message = message;
        };
        [[nodiscard]] const char* what() const noexcept override {
            return this->m_message.c_str();
        };

    private:
        std::string m_message;
};

class PipeServer {
    public:
        explicit PipeServer(const std::string& pipeName);
        ~PipeServer();

        void write(const std::string& message) const;
        Message read() const;

    private:
        std::string m_pipeName;
#ifdef WIN32
        HANDLE m_pipe;
#else
        int m_pipe;
#endif
};

#endif //PIPESERVER_HPP
