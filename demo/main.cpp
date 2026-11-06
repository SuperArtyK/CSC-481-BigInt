#include <iostream>

int main()
{
    //Simple version of 4.7 Example session
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);
    std::cout << "You entered: " << input << std::endl;

    return 0;
}
