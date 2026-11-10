
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
    
    dprintf("Answer to life, universe, and everything: %d", 42);
    
    BigInt b;
    BigInt a = std::move(b);
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
