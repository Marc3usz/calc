#include "functionFactory.hpp"

#include <stdexcept>
#include <fmt/base.h>
#include <cctype>
#include <algorithm>
#include <cmath>

#include "derivative.hpp"

static int safeGet(std::map<std::string, int> map, std::string key, int placeholder = 0) {
	auto res = map.find(key);
	if (res == map.end()) return placeholder; else return res->second;
}

void FunctionFactory::tokenize(std::string& expression) {
	std::string expressionNoSpaces;
	for (char c : expression) {
		if (c != ' ') {
			expressionNoSpaces += c;
		}
	}
	size_t i = 0;
	while (i < expressionNoSpaces.length()) {
		char c = expressionNoSpaces[i];
		if (isdigit(c) || c == '.') {

			std::string number;
			bool hasDecimal = false;

			if (c == '.') {
				number += "0.";
				hasDecimal = true;
				i++;
			}
			else {
				number += c;
				i++;
			}

			while (i < expressionNoSpaces.length() &&
				(isdigit(expressionNoSpaces[i]) || expressionNoSpaces[i] == '.')) {
				if (expressionNoSpaces[i] == '.') {
					if (hasDecimal)
						throw std::invalid_argument("Invalid number with 2+ decimal points");
					hasDecimal = true;
				}
				number += expressionNoSpaces[i];
				i++;
			}

			tokens.push_back(number);
			i--; 
		}
		else if (c == 'x') {
			tokens.push_back("x");
		}
		else if (isalpha(c) || c == '\'') {
			std::string identifier;
			identifier += c;
			size_t j = i + 1;
			while (j < expressionNoSpaces.length() && (isalpha(expressionNoSpaces[j]) || expressionNoSpaces[j] == '\''))
				identifier += expressionNoSpaces[j++];

			if (j < expressionNoSpaces.length() && expressionNoSpaces[j] == '\'' && j + 1 < expressionNoSpaces.length() && expressionNoSpaces[j + 1] == '(') {
				tokens.push_back(identifier + "'");
				i = j;
			}
			else if (j < expressionNoSpaces.length() && expressionNoSpaces[j] == '(') {
				tokens.push_back(identifier);
				i = j - 1;
			}
			else {
				throw std::invalid_argument("Invalid identifier found: " + identifier);
			}
		}
		else if (std::string("+-/*^()").find(c) != std::string::npos)
			tokens.push_back(std::string() + c);
		else
			throw std::invalid_argument("Invalid token found: " + std::string(1, c));
		i++;
	}
}


void FunctionFactory::parse() {
	std::map<std::string, int> precedence = {
		{"+", 1},
		{"-", 1},
		{"*", 2},
		{"/", 2},
		{"^", 3}
	};

	std::stack<std::string> operators;

	size_t i = 0;
	while (i < tokens.size()) {
		std::string token = tokens[i];

		if (token == "x" ||
			(!token.empty() && [&token]() {
				bool hasDecimal = false;
				for (char c : token) {
					if (c == '.') {
						if (hasDecimal) return false;
						hasDecimal = true;
					}
					else if (!std::isdigit(c)) {
						return false;
					}
				}
				return true;
				}())) {
			parsed.push(token);
		}
		else if (!token.empty() &&
			std::all_of(token.begin(), token.end(), [](char c) { return std::isalpha(c) or c == '\''; }) &&
			token != "x" &&
			i + 1 < tokens.size() &&
			tokens[i + 1] == "(") {
			operators.push(token);
		}
		else if (token == "(") {
			operators.push(token);
		}
		else if (token == ")") {
			while (!operators.empty() && operators.top() != "(") {
				parsed.push(operators.top());
				operators.pop();
			}

			if (!operators.empty() && operators.top() == "(") {
				operators.pop();

				if (!operators.empty() &&
					std::all_of(operators.top().begin(), operators.top().end(),
						[](char c) { return std::isalpha(c); }) &&
					operators.top() != "x") {
					parsed.push(operators.top());
					operators.pop();
				}
			}
		}
		else if (precedence.find(token) != precedence.end()) {
			while (!operators.empty() &&
				operators.top() != "(" &&
				precedence.find(operators.top()) == precedence.end()) {
				parsed.push(operators.top());
				operators.pop();
			}

			while (!operators.empty() &&
				operators.top() != "(" &&
				precedence.find(operators.top()) != precedence.end() &&
				((token != "^" && precedence[operators.top()] >= precedence[token]) ||
					(token == "^" && precedence[operators.top()] > precedence[token]))) {
				parsed.push(operators.top());
				operators.pop();
			}

			operators.push(token);
		}

		i++;
	}

	while (!operators.empty()) {
		parsed.push(operators.top());
		operators.pop();
	}
}
Function FunctionFactory::buildFunction(char identifier) {
	std::stack<Function> fnStack;

	std::queue<std::string> parsedCopy = parsed;

	while (!parsedCopy.empty()) {
		std::string token = parsedCopy.front();

		parsedCopy.pop();

		if (token == "x") {
			fnStack.push([](ld x) { return x; });
		}
		else if (!token.empty() && [&token]() {
			try {
				std::size_t pos = 0;
				std::stold(token, &pos);
				return pos == token.length();
			}
			catch (const std::exception&) {
				return false;
			}
			}()) {
			ld val = std::stold(token);
			fnStack.push([val](ld x) { return val; });
		}
		else if (token == "+") {
			if (fnStack.size() < 2)
				throw std::runtime_error("Expression parsing failed: not enough operands for +");
			Function rhs = fnStack.top(); fnStack.pop();
			Function lhs = fnStack.top(); fnStack.pop();
			fnStack.push([lhs, rhs](ld x) { return lhs(x) + rhs(x); });
		}
		else if (token == "-") {
			if (fnStack.size() < 2)
				throw std::runtime_error("Expression parsing failed: not enough operands for -");
			Function rhs = fnStack.top(); fnStack.pop();
			Function lhs = fnStack.top(); fnStack.pop();
			fnStack.push([lhs, rhs](ld x) { return lhs(x) - rhs(x); });
		}
		else if (token == "*") {
			if (fnStack.size() < 2)
				throw std::runtime_error("Expression parsing failed: not enough operands for *");
			Function rhs = fnStack.top(); fnStack.pop();
			Function lhs = fnStack.top(); fnStack.pop();
			fnStack.push([lhs, rhs](ld x) { return lhs(x) * rhs(x); });
		}
		else if (token == "/") {
			if (fnStack.size() < 2)
				throw std::runtime_error("Expression parsing failed: not enough operands for /");
			Function rhs = fnStack.top(); fnStack.pop();
			Function lhs = fnStack.top(); fnStack.pop();
			fnStack.push([lhs, rhs](ld x) { return lhs(x) / rhs(x); });
		}
		else if (token == "^") {
			if (fnStack.size() < 2)
				throw std::runtime_error("Expression parsing failed: not enough operands for ^");
			Function rhs = fnStack.top(); fnStack.pop();
			Function lhs = fnStack.top(); fnStack.pop();
			fnStack.push([lhs, rhs](ld x) { return std::pow(lhs(x), rhs(x)); });
		}
		else if (token.back() == '\'') {
			if (fnStack.empty())
				throw std::runtime_error("Illegal derivative: stack is empty");

			Function arg = fnStack.top(); fnStack.pop();

			std::string _identifier = token.substr(0, token.size() - 1);

			if (_identifier.size() == 1 and _identifier[0] >= identifier)
				throw std::invalid_argument("User-defined function calls must be to preceding or builtin functions");

			Function fn;
			auto builtinIt = builtInFunctions.find(_identifier);
			if (builtinIt != builtInFunctions.end()) {
				fn = builtinIt->second;
			}
			else {
				auto userIt = functions.find(_identifier);
				if (userIt == functions.end())
					throw std::invalid_argument("Invalid function identifier: " + _identifier);
				fn = userIt->second;
			}

			Function fn_prime = derivative(fn);

			fnStack.push([fn_prime, arg](ld x) { ld z = arg(x); return fn_prime(z); });
		}
		else if (std::all_of(token.begin(), token.end(), ::isalpha)) {
			if (fnStack.empty())
				throw std::runtime_error("Function call requires an argument on the stack");

			Function arg = fnStack.top(); fnStack.pop();

			if (token.size() == 1 and token[0] >= identifier)
				throw std::invalid_argument("User-defined function calls must be to preceding or builtin functions");

			Function fn;
			auto builtinIt = builtInFunctions.find(token);
			if (builtinIt != builtInFunctions.end()) {
				fn = builtinIt->second;
			}
			else {
				auto userIt = functions.find(token);
				if (userIt == functions.end())
					throw std::invalid_argument("Invalid function identifier: " + token);
				fn = userIt->second;
			}

			fnStack.push([fn, arg](ld x) { return fn(arg(x)); });
		}
	}

	if (fnStack.empty())
		throw std::runtime_error("Function construction failed: empty result");

	return fnStack.top();
}
void FunctionFactory::loadFunctions(strvecr strfns)
{
	for (std::string str : strfns) {
		char identifier = str[0];
		std::string body = str.substr(1, str.size());
		savedStrs[identifier] = body;
		parseFunction(body, identifier);
	}
}
FunctionFactory::FunctionFactory(functionMapping& fns) : builtInFunctions(fns) {};
FunctionFactory::FunctionFactory(functionMapping& fns, strvecr strfns) : builtInFunctions(fns)
{
	loadFunctions(strfns);
}
const functionMapping& FunctionFactory::getFunctions() {
	return functions;
};
void FunctionFactory::parseFunction(std::string expression, char identifier) 
{
	try {
	tokenize(expression);
	parse();

	Function fn = buildFunction(identifier);
	functions[std::string() + identifier] = fn;
	savedStrs[identifier] = expression;

	parsed = std::queue<std::string>();
	tokens = std::vector<std::string>();
	}
	catch (std::exception& e) {
		parsed = std::queue<std::string>();
		tokens = std::vector<std::string>();
		throw e;
	}
}
std::vector<std::string> FunctionFactory::exportFunctions()
{
	std::vector<std::string> res;
	for (auto pair : savedStrs) {
		res.push_back(pair.first + pair.second);
	}
	sortStrVecByFirstChar(res);
	return res;
};
void FunctionFactory::importFunctions(strvecr fnstrs) {
	loadFunctions(fnstrs);
}