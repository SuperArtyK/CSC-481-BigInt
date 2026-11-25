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

	cout << "Decimal:    " << a.toStringDec() << NLC;
	cout << "Scientific: " << a.toStringSci(20, true) << NLC;
	cout << "Default:    " << a.toString() << NLC;
	cout << "ostream<<:  " << a << NLC;

	int repetitions = 5;
	for (int i = 0; i < repetitions; i++) {
		cout << "Enter the number (" << i + 1 << " out of " << repetitions << "): ";
		cin >> a;
		if (cin.fail()) {
			cout << "Wrong input, bigint could not be parsed" << NLC;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else {
			cout << "Inputted bigint: " << a << NLC;
		}
	}


	return 0;
}
