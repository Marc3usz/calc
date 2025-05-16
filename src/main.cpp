#include "functionFactory.hpp"
#include <iostream>

int main() {
	functionMapping fs = {
		{"sin", [](ld x) {return std::sin(x); }},
		{"cos", [](ld x) {return std::cos(x); }}
	};
	try
	{
		FunctionFactory fns(fs);
		std::string expr = "sin'(x) - cos(x)";
		fns.parseFunction(expr, 'a');
		std::string expr2 = "a(x) - f'(x)";
		fns.parseFunction(expr2, 'b');
		std::map<std::string, Function> functions = fns.getFunctions();
		Function fnA = functions.find("b")->second;
		std::cout << fnA(5) << "\n" << fnA(6) << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	return 0; 
}