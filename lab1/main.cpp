#include <iostream>
#include <vector>
#include <string>

int main()
{
    // Program initialization
    std::cout << "Hello, World!" << std::endl;

    // Variables declaration
    int number = 0;
    std::string input;
    std::vector<int> data;

    // Input example
    std::cout << "Enter a number: ";
    std::cin >> number;

    // Processing
    std::cout << "You entered: " << number << std::endl;

    return 0;
}