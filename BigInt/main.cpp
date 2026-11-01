
#define DEBUGMODE 1
#include <iostream>
#include "bigint/utils.hpp"
#include "bigint/bigint.hpp"



int main() {
    cdbg << "Hello from BigInt!\n";
    
    dprintf("Answer to life, universe, and everything: %d", 42);
    
    BigInt a;
    
    cdbg<<powersOf10[0]<<NLC;
    cdbg<<powersOf10[18]<<NLC;
    
    std::cout<<NLC;
    
    cdbg<<digitCount<char>(128)<<NLC;
    
    a.setLimb(1000, 1);
    cdbg<<a.getLimbCount()<<NLC;
    cdbg<<a.getDigitCount()<<NLC;
    cdbg<<a.getFirstLimb()<<NLC;
    cdbg<<a.getLastLimb()<<NLC;
    cdbg<<a.getLimb(a.getLimbCount()-1)<<NLC;
    cdbg<<boolToString(a.isNegative())<<NLC;
    cdbg<<boolToString(a.isZero())<<NLC;
    
    return 0;
}
