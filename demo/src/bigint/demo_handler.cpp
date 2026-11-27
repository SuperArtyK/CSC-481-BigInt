#include "bigint/demo_accessors.hpp"
#include <string>


Calculator::Calculator() : ans(0), hasAnswer(false) {}

bool exitCommand(const std::string& equation) {
    static const std::regex exit_pattern(R"(\s*@exit\s*)", std::regex_constants::icase);
    return std::regex_match(equation, exit_pattern);
}

bool showVar(const std::string& equation, std::string& varName) {
    static const std::regex showVar_pattern(R"(\s*@show[ \t]+([a-zA-Z0-9]*)\s*)", std::regex_constants::icase);
    std::smatch match;
    if (std::regex_match(equation, match, showVar_pattern)) {
        varName = match[1].str();
        return true;
    }
    return false;
}

//filter through allowed symbols
static int parseNumber(const std::string& text) {
    try {
        std::size_t charactersRead = 0;
        int value = std::stoll(text, &charactersRead);
        if (charactersRead != text.size()) {
            throw std::invalid_argument("Invalid number");
        }
        return value;
    }
    catch (const std::invalid_argument&) {
        throw std::invalid_argument("Cannot convert '" + text + "' to a number");
    }
}

int Calculator::operationHandle(const std::string& num1, char op, const std::string& num2) {
    if (op == '=') {
        varString = num1;

        if (num2 == "ans") {
            if (!hasAnswer) {
                throw std::invalid_argument("There is no previous answer");
            }
            value = ans;
        }
        else if (customVars.contains(num2))
            {
                value = customVars[num2];
            }
        else {
            value = parseNumber(num2);
        }

        customVars[varString] = value;
        return value;
    }

    if (num1 == "ans") {
        if (!hasAnswer) {
            throw std::invalid_argument("There is no previous answer");
        }
        left = ans;
    } else if (customVars.contains(num1)) {
        left = customVars[num1];
    } else {
        left = parseNumber(num1);
    }

    if (num2 == "ans") {
        if (!hasAnswer) {
            throw std::invalid_argument("There is no previous answer");
        }
        right = ans;
    } else if (customVars.contains(num2)) {
        right = customVars[num2];
    } else {
        right = parseNumber(num2);
    }

    int result;

    switch (op) {
        case '+': result = left + right; break;
        case '-': result = left - right; break;
        case '*': result = left * right; break;
        case '%': result = left % right; break;
        case '/':
            if (right == 0) {
                throw std::invalid_argument("Cannot divide by zero");
            }
            result = left/right;
            break;
        default:
            throw std::invalid_argument("Unrecognized operation");
    }

    ans = result;
    hasAnswer = true;
    return ans;
}

int Calculator::getVariable(const std::string& name) const{
    if (name =="ans") {
        if (!hasAnswer)
        {
            throw std::invalid_argument("There is no previous answer");
        }
        return ans;
    }
    auto it = customVars.find(name);

    if (it == customVars.end())
    {
        throw std::invalid_argument("Variable " + name + " does not exist");
    }
    return it->second;
}