#include "bigint/demo_accessors.hpp"
#include <exception>
#include <iostream>
#include <regex>

//ToDo: cleanup main.cpp code
int main()
{
    Calculator calc;
    std::string equation;
    int ans = 0;

    while (true)
    {
        std::cout << "Enter an equation: ";
        if (!std::getline(std::cin, equation))
        {
            break;
        }

        if (exitCommand(equation)) {
            break;
        }

        if (showVar(equation)) {
            std::cout << "ans = " << ans << std::endl;
            continue;
        }
        
        //This is a comment to showcase my verison of the code. Please work o nthe version you feel more confortable with.
        /*
        if (std::regex_match(equation, std::regex("@show[ \t]+ans", std ::regex_constants::icase))) {
            std::cout << "ans = " << ans << std::endl;
            continue;
        }
        */
        
        std::smatch parts;
        if (!std::regex_match(equation, parts, format))
        {
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

        try
        {
            ans = calc.operationHandle(num1, op, num2);
            std::cout << "result: " << ans << std::endl;
        }
        catch (const std::exception& error)
        {
            std::cerr << "ERROR:" << error.what() << std::endl;
                return 1;
        }

        std::cout << "ans = " << ans << std::endl;
    }

    return 0;
}
