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

static int resolveOperand(const std::string& text, int ans, bool hasAnswer,const std::map<std::string, int>& customVars){
    bool negative = false;
    std::string name = text;

    if (!name.empty() && (name[0] == '-'|| name[0] == '+'))
    {
        negative = (name[0] == '-');
        name = name.substr(1);
    }
    int result;

    if (name == "ans") {
        if (!hasAnswer) {
            throw std::invalid_argument("There is no previous answer");
        }
        result = ans;
    }
    else if (customVars.contains(name)) {
        result = customVars.at(name);
    }
    else {
        result = parseNumber(name);
    }

    if (negative) {
        result = -result;
    }
    return result;
}
int Calculator::operationHandle(const std::string& num1, char op, const std::string& num2) {
    if (op == '=') {
        if (!num1.empty() && (num1[0]== '+'|| num1[0] == '-'))
        {
            throw std::invalid_argument("Invalid assignment variable");
        }
        varString = num1;
        value =resolveOperand(num2, ans, hasAnswer, customVars);

        customVars[varString] = value;
        return value;
    }
    left = resolveOperand(num1, ans, hasAnswer, customVars);
    right = resolveOperand(num2, ans, hasAnswer, customVars);

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