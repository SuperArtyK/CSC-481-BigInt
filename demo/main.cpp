#include "bigint/demo_accessors.hpp"
#include <exception>
#include <iostream>
#include <regex>

int main() {
    Calculator calc;
    std::string equation;
    int ans = 0;

    //filter through allowed symbols
    const std::regex format(
        R"(^\s*(ans|-?\d+)\s*([+*/-])\s*(ans|-?\d+)\s*$)");

    while (true) {
        std::cout << "Enter an equation: ";
        if (!std::getline(std::cin, equation)) {
            break;
        }

        std::smatch parts;
        if (!std::regex_match(equation, parts, format)) {
            std::cout << "Please Enter a Valid Operation in Format: [number] [arithmetic] [number]" << std::endl;
            continue;
        }

        std::string num1 = parts[1].str();
        char op = parts[2].str()[0];
        std::string num2 = parts[3].str();

        std::cout << "parsed input:" << std::endl;
        std::cout << "num1: " << num1 << std::endl;
        std::cout << "operation: " << op << std::endl;
        std::cout << "num2: " << num2 << std::endl;

        try {
            ans = calc.operationHandle(num1, op, num2);
            std::cout << "result: " << ans << std::endl;
        } catch (const std::exception& error) {
            std::cout << "Error Invalid Argument" << std::endl;
        }

        std::cout << "ans = " << ans << std::endl;
    }

    return 0;
}