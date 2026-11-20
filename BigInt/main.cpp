
#define DEBUGMODE 1
#include <iostream>
#include "bigint/utils.hpp"
#include "bigint/bigint.hpp"

#include <cinttypes>
#include <cstdint>
#include <cstdlib>

using std::cout;
using std::endl;
using std::cin;

int main() {
    cdbg << "Hello from BigInt!\n";
    cdbg<<(std::is_same<typename std::remove_cv<const bool>::type, bool>::value)<<NLC;
    dprintf("Answer to life, universe, and everything: %d", 42);
    
    BigInt b = (unsigned long long)123123123123;
    b = 1234;
    cout << b.toStringDec()<<NLC;
    BigInt c("0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999");
    cout<<c.toStringDec()<<NLC;
    c = "9999999999888888888877777777776666666666555555555544444444443333333333222222222211111111110000000000";
    cout<<c.toStringDec()<<NLC;
    BigInt a = std::move(b);
    a = std::move(c);
    std::srand(4);
    
    cout<<powersOf10[0]<<NLC;
    cout<<powersOf10[18]<<NLC;
    
    std::cout<<NLC;
    
    cdbg<<digitCount<int>(INT_MAX)<<NLC;
    
    for (int i = 0; i < 12; i++) {
        a.setLimb(i, (limb_t(std::rand())*powersOf10[LIMB_DIGITS/2]+ std::rand())  % STORAGE_MASK);
    }
    
    cout<<a.getLimbCount()<<NLC;
    cout<<a.getDigitCount()<<NLC;
    cout<<a.getFirstLimb()<<NLC;
    cout<<a.getLastLimb()<<NLC;
    cout<<a.getLimb(a.getLimbCount()-1)<<NLC;
    cout<<boolToString(a.isNegative())<<NLC;
    cout<<boolToString(a.isZero())<<NLC;
    cout<<boolToString(a.isNormalised())<<NLC;
    
    cout<<NLC<<NLC<<a.toStringDec()<<NLC;
    
    
    
    return 0;
}
