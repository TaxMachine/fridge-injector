//
// Created by taxis on 2023-11-20.
//

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <string>

#include "../injection/pipeserver.hpp"

class Console {
    public:
        explicit Console(PipeServer* pipeServer);
        ~Console();

        void render();
        void WaitForLogAndPrint();
        void write(const Message& msg);

    private:
        PipeServer* m_pipeServer;
        std::string m_buffer;
        bool m_scrollToBottom;
};

#endif //CONSOLE_HPP
