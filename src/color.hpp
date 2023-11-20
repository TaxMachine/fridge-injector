//
// Created by taxis on 2023-11-16.
//

#ifndef COLOR_HPP
#define COLOR_HPP

#include <string>

#include "imgui.h"

class Color {
    public:
        Color(const float& r, const float& g, const float& b, const float& a);
        ~Color() = default;

        [[nodiscard]] std::string terminal() const;
        [[nodiscard]] ImVec4 toImVec4() const;

        [[nodiscard]] float r() const;
        [[nodiscard]] float g() const;
        [[nodiscard]] float b() const;
        [[nodiscard]] float a() const;

        static inline const std::string RESET = "\33[0m";
        static inline const std::string RGB = "\33[38;2;";

    private:
        float m_r;
        float m_g;
        float m_b;
        float m_a;
};

#endif //COLOR_HPP
