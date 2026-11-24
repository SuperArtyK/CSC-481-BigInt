#ifndef DEMO_ACCESSORS_HPP
#define DEMO_ACCESSORS_HPP

#include <string>
#include <regex>

//filter through allowed symbols
inline const std::regex format(R"(^\s*(ans|-?\d+)\s*([+*%/-])\s*(ans|-?\d+)\s*$)");

bool exitCommand(const std::string& equation);
bool showVar(const std::string& equation);

class Calculator {
private:
    int ans;
    bool hasAnswer;

public:
    Calculator();
    int operationHandle(const std::string& num1, char op, const std::string& num2);
};

#endif
