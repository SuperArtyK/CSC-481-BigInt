#ifndef DEMO_ACCESSORS_HPP
#define DEMO_ACCESSORS_HPP

#include <string>
#include <regex>
#include <map>

//filter through allowed symbols
inline const std::regex format(R"(^\s*([+-]?(?:[a-zA-Z][a-zA-Z0-9]*|\d+))\s*([+*%/=-])\s*([+-]?(?:[a-zA-Z][a-zA-Z0-9]*|\d+))\s*$)");

bool exitCommand(const std::string& equation);
bool showVar(const std::string& equation, std::string& varName);

class Calculator {
private:
    int ans, value, left, right;
    bool hasAnswer;
    std::string varString;
    std::map<std::string, int> customVars;


public:
    Calculator();
    int operationHandle(const std::string& num1, char op, const std::string& num2);
    int getVariable(const std::string& name) const;
};

#endif
