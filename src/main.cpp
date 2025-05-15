#include "functionFactory.hpp"
#include <iostream>

int main() {

	try
	{
		FunctionFactory fns;
		std::string expr = "3* x^2 -5";
		fns.parseFunction(expr, 'a');
		std::map<std::string, Function> functions = fns.getFunctions();
		Function fnA = functions.find("a")->second;
		std::cout << fnA(5) << "\n" << fnA(6) << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	return 0; 
}