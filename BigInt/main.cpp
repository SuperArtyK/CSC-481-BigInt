
#define DEBUGMODE 1
#include <iostream>
#include "bigint/utils.hpp"
#include "bigint/bigint.hpp"

#include <cinttypes>
#include <cstdint>


int main() {
    using namespace std;
    cdbg << "Hello from BigInt!\n";

    dprintf("Answer to life, universe, and everything: %d", 42);

    BigInt a;

    cdbg<<powersOf10[0]<<NLC;
    cdbg<<powersOf10[18]<<NLC;

    std::cout<<NLC;

    cdbg<<digitCount<char>(128)<<NLC;

    unsigned index = 0;
    std::string input_value;

    std::cout << "Select a number: ";
    std::cin >> input_value;
    int len = input_value.length();
    cdbg << "Length: " << len << NLC;

    while (!input_value.empty()) {
        unsigned long long safe_value;
        if (input_value.length() <= 18) {
            safe_value = std::stoull(input_value);
            input_value = "";
        } else {
            std::string righter_part = input_value.substr(input_value.length() - 18);
            safe_value = std::stoull(righter_part);
            input_value = input_value.substr(0, input_value.length() - 18);
        }
        a.setLimb(index, safe_value);
        index++;
    }


    cdbg << "Total limbs: " << index << NLC;
    cdbg<<a.getLimbCount()<<NLC;
    cdbg<<a.getDigitCount()<<NLC;
    cdbg<<a.getFirstLimb()<<NLC;
    cdbg<<a.getLastLimb()<<NLC;
    cdbg<<a.getLimb(a.getLimbCount()-1)<<NLC;
    cdbg<<boolToString(a.isNegative())<<NLC;
    cdbg<<boolToString(a.isZero())<<NLC;

    cdbg<<NLC<<NLC<<a.toStringDec()<<NLC;
    return 0;
}
