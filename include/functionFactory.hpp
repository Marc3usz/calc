#pragma once

#include <string>
#include <stack>
#include <queue>
#include <map>
#include <variant>

#include "common.hpp"

typedef std::map<std::string, Function> functionMapping;

class FunctionFactory {
private:
	void tokenize(std::string&);
	void parse();
	Function buildFunction(char);
	functionMapping functions;
	std::queue<std::string> parsed;
	std::vector<std::string> tokens;
public:
	FunctionFactory() = default;
	~FunctionFactory() = default;
	functionMapping& getFunctions();
	void parseFunction(std::string& expression, char identifier);
};