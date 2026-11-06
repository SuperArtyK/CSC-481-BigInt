
#define DEBUGMODE 1
#include <iostream>
#include "bigint/utils.hpp"
#include "bigint/bigint.hpp"

#include <cinttypes>
#include <cstdint>

using std::cout;
using std::endl;
using std::cin;

int main() {
    cdbg << "Hello from BigInt!\n";
    
    dprintf("Answer to life, universe, and everything: %d", 42);
    
    BigInt a;
    
    cout<<powersOf10[0]<<NLC;
    cout<<powersOf10[18]<<NLC;
    
    std::cout<<NLC;
    
    cdbg<<digitCount<int>(INT_MAX)<<NLC;
    
    a.setLimb(1, 1);
    cout<<a.getLimbCount()<<NLC;
    cout<<a.getDigitCount()<<NLC;
    cout<<a.getFirstLimb()<<NLC;
    cout<<a.getLastLimb()<<NLC;
    cout<<a.getLimb(a.getLimbCount()-1)<<NLC;
    cout<<boolToString(a.isNegative())<<NLC;
    cout<<boolToString(a.isZero())<<NLC;
    
    cout<<NLC<<NLC<<a.toStringDec()<<NLC;
    
    
    
    return 0;
}
