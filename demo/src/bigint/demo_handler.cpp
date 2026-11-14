#include "bigint/demo_accessors.hpp"
#include <stdexcept>
#include <string>
#include <regex>


Calculator::Calculator() : ans(0), hasAnswer(false) {}

static int parseNumber(const std::string& text) {
    std::size_t charactersRead = 0;
    int value = std::stoll(text, &charactersRead);

    if (charactersRead != text.size()) {
        throw std::invalid_argument("Invalid number");
    }

    return value;
}

int Calculator::operationHandle(const std::string& num1, char op, const std::string& num2) {
    int left;
    if (num1 == "ans") {
        if (!hasAnswer) {
            throw std::invalid_argument("There is no previous answer");
        }
        left = ans;
    } else {
        left = parseNumber(num1);
    }

    int right;
    if (num2 == "ans") {
        if (!hasAnswer) {
            throw std::invalid_argument("There is no previous answer");
        }
        right = ans;
    }else {
        right = parseNumber(num2);
    }

    int result;

    switch (op) {
        case '+': result = left + right; break;
        case '-': result = left - right; break;
        case '*': result = left * right; break;
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