#pragma once

#include <string>
#include <stack>
#include <queue>
#include <map>

#include "common.hpp"

typedef std::map<std::string, Function> functionMapping;

class FunctionFactory {
private:
	void tokenize(std::string&);
	void parse();
	Function buildFunction(char);
	functionMapping functions;
	functionMapping builtInFunctions;
	std::queue<std::string> parsed;
	std::vector<std::string> tokens;
	std::map<char, std::string> savedStrs;
	void loadFunctions(strvecr);
public:
	FunctionFactory() = default;
	FunctionFactory(functionMapping&);
	FunctionFactory(functionMapping&, strvecr);
	~FunctionFactory() = default;
	const functionMapping& getFunctions();
	void parseFunction(std::string expression, char identifier);
	std::vector<std::string> exportFunctions();
	void importFunctions(strvecr);
};