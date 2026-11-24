#ifndef DEMO_ACCESSORS_HPP
#define DEMO_ACCESSORS_HPP

#include <string>

//filter through allowed symbols
inline const std::regex format(R"(^\s*(ans|-?\d+)\s*([+*%/-])\s*(ans|-?\d+)\s*$)");

class Calculator {
private:
    int ans;
    bool hasAnswer;

public:
    Calculator();
    int operationHandle(const std::string& num1, char op, const std::string& num2);
};

#endif
