//
// Created by TaxMachine on 2023-11-12.
//

#ifndef FRIDGE_INJECTOR_EXCEPTIONS_HPP
#define FRIDGE_INJECTOR_EXCEPTIONS_HPP

#include <stdexcept>

class InjectionException final : public std::runtime_error {
    public:
        explicit InjectionException(const std::string& message) : std::runtime_error(message) {}

        explicit InjectionException(const char* message) : std::runtime_error(message) {}

        ~InjectionException() override = default;

        [[nodiscard]] const char* what() const noexcept override {
            return std::runtime_error::what();
        }
};

class ConfigException final : public std::runtime_error {
    public:
        explicit ConfigException(const std::string& message) : std::runtime_error(message) {}

        explicit ConfigException(const char* message) : std::runtime_error(message) {}

        ~ConfigException() override = default;

        [[nodiscard]] const char* what() const noexcept override {
            return std::runtime_error::what();
        }
};

class NotImplementedException final : public std::runtime_error {
    public:
        explicit NotImplementedException(const std::string& message) : std::runtime_error(message) {}

        explicit NotImplementedException(const char* message) : std::runtime_error(message) {}

        ~NotImplementedException() override = default;

        [[nodiscard]] const char* what() const noexcept override {
            return std::runtime_error::what();
        }
};

#endif //FRIDGE_INJECTOR_EXCEPTIONS_HPP
