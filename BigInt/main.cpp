
#define DEBUGMODE 1
#include <iostream>
#include "bigint/utils.hpp"
#include "bigint/bigint.hpp"



int main() {
    cdbg << "Hello from BigInt!\n";
    
    dprintf("Answer to life, universe, and everything: %d", 42);
    
    BigInt a;
    
    return 0;
}
