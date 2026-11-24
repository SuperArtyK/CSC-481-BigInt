
#include <iostream>
#include "bigint/bigint.hpp"

#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <float.h>

using std::cout;
using std::endl;
using std::cin;

int main() {
    
    
    BigInt a("1234567890123456789012345678901234567890");
    
    cout<<"Decimal:    "<<a.toStringDec()<<NLC;
    cout<<"Scientific: "<<a.toStringSci(20, true)<<NLC;
    cout<<"Default:    "<<a.toString()<<NLC;
    
    
    return 0;
}
