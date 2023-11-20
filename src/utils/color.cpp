//
// Created by taxis on 2023-11-16.
//

#include "color.hpp"

Color::Color(const float& r, const float& g, const float& b, const float& a) {
    this->m_r = r;
    this->m_g = g;
    this->m_b = b;
    this->m_a = a;
}

std::string Color::terminal() const {
    return RGB + std::to_string(this->m_r) + ";" +
                        std::to_string(this->m_g) + ";" +
                        std::to_string(this->m_b) + "m";
}

ImVec4 Color::toImVec4() const {
    return {this->m_r, this->m_g, this->m_b, this->m_a};
}

float Color::r() const {
    return this->m_r;
}

float Color::g() const {
    return this->m_g;
}

float Color::b() const {
    return this->m_b;
}

float Color::a() const {
    return this->m_a;
}