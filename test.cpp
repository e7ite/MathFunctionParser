#include "parser.h"
#include <iostream>

int main()
{
	int num;

	std::string tmp;
	std::cout << "Enter a function: ";
	std::cin >> tmp;
	std::cout << "Enter input to f";
	std::cin >> num;

    Function f(tmp);
	std::cout << f.Solve(num) << std::endl;
}