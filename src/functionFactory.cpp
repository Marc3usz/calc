#include "functionFactory.hpp"

#include <stdexcept>
#include <fmt/base.h>
#include <cctype>
#include <algorithm>
#include <cmath>

#include "derivative.hpp"

int safeGet(std::map<std::string, int> map, std::string key, int placeholder = 0) {
	auto res = map.find(key);
	if (res == map.end()) return placeholder; else return res->second;
}

class FunctionFactory {
private:
	void tokenize(std::string& expression) {

		std::string expressionNoSpaces;
		for (char c : expression) {
			if (c != ' ') {
				expressionNoSpaces += c;
			}
		}

		size_t i = 0;
		while (i < expressionNoSpaces.length()) {
			char c = expressionNoSpaces[i];
			if (isdigit(c) or c == '.') {
				std::string number;
				bool hasDecimal = false;

				if (c == '.') {
					if (i + 1 < expressionNoSpaces.length() and isdigit(expressionNoSpaces[i + 1])) {
						number = "0";
						hasDecimal = true;
						i++;
						c = expressionNoSpaces[i];
					}
				}

				else {
					i++;
					tokens.push_back(std::string() + c);
					continue;
				}

				while (i < expressionNoSpaces.length() and (isdigit(expressionNoSpaces[i]) or expressionNoSpaces[i] == '.')) {
					if (expressionNoSpaces[i] == '.') {
						if (hasDecimal)
							throw std::invalid_argument("Invalid number with 2+ decimal points");
						hasDecimal = true;
					}
					number += expressionNoSpaces[i];
					i++;
				}

				if (!number.empty())
					tokens.push_back(number);

				i--;
			}
			else if (c == 'x') {
				tokens.push_back("x");
			}
			else if (isalpha(c)) {
				std::string identifier;
				identifier += c;
				size_t j = i + 1;
				while (j < expressionNoSpaces.length() and isalpha(expressionNoSpaces[j]))
					identifier += expressionNoSpaces[j++];

				// pochodne
				if (j + 1 < expressionNoSpaces.length() and expressionNoSpaces[j] == '\'' and expressionNoSpaces[j + 1] == '(') {
					tokens.push_back(identifier + "'");
					i = j;
				}
				else if (j < expressionNoSpaces.length() and expressionNoSpaces[j] == '(') {
					tokens.push_back(identifier);
					i = j - 1;
				}
				else {
					throw std::invalid_argument("Invalid identifier found");
				}
			}
			else if (std::string("+-/*^()").find(c))
				tokens.push_back(std::string() + c);
			else
				throw std::invalid_argument("Invalid token found");
			i++;
		}
	};


	void parse() {
		std::map<std::string, int> precedence = {
			{"+", 1},
			{"-", 1},
			{"*", 2},
			{"/", 3},
			{"^", 4}
		};

		std::stack<std::string> operators;

		size_t i = 0;
		while (i < tokens.size()) {
			std::string token = tokens[i];

			if (std::all_of(token.begin(), token.end(), ::isdigit) or token.find(".") or token == "x")
				parsed.push(token);
			else if (std::all_of(token.begin(), token.end(), ::isalpha) and i + 1 < tokens.size() and tokens[i + 1] == "(")
				operators.push(token);
			else if (token == "(")
				operators.push("(");
			else if (token == ")") {
				while (operators.size() and operators.top() != "(")
					parsed.push(operators.top()), operators.pop();
				if (operators.size() and operators.top() == "(")
				{
					operators.pop();
					if (operators.size() and std::all_of(operators.top().begin(), operators.top().end(), ::isalpha) and operators.top() != "x")
						parsed.push(operators.top()), operators.pop();
				}
			}
			else if (precedence.find(token) != precedence.end()) {
				while (operators.size() and operators.top() != "(" and precedence.find(operators.top()) != precedence.end())
					parsed.push(operators.top()), operators.pop();
				while (operators.size() and operators.top() != "(" and precedence.find(operators.top()) == precedence.end() and (
					(token != "^" and safeGet(precedence, operators.top()) >= safeGet(precedence, token)) or
					token == "^" and safeGet(precedence, operators.top()) > safeGet(precedence, token)))
					parsed.push(operators.top()), operators.pop();
				operators.push(token);
			}
			i++;
		}
		while (operators.size())
			parsed.push(operators.top()), operators.pop();
 	
	};
	Function buildFunction(char identifier) {
		std::stack<Function> fnStack;
		while (parsed.size()) {
			std::string token = parsed.front(); parsed.pop();
			if (token == "x")
				fnStack.push([](ld x) {return x; });
			else if (std::all_of(token.begin(), token.end(), [](char c) {return c == '.' or isalpha(c); })) {
				ld val = std::stold(token);
				fnStack.push([val](ld x) {return val; });
			}
			else if (token == "+") {
				if (parsed.size() < 2)
					throw std::runtime_error("Expression parsing failed");
				Function rhs = fnStack.top(); fnStack.pop();
				Function lhs = fnStack.top(); fnStack.pop();
				fnStack.push([&lhs, &rhs](ld x) {return lhs(x) + rhs(x); });
			}
			else if (token == "-") {
				if (parsed.size() < 2)
					throw std::runtime_error("Expression parsing failed");
				Function rhs = fnStack.top(); fnStack.pop();
				Function lhs = fnStack.top(); fnStack.pop();
				fnStack.push([&lhs, &rhs](ld x) {return lhs(x) - rhs(x); });
			}
			else if (token == "*") {
				if (parsed.size() < 2)
					throw std::runtime_error("Expression parsing failed");
				Function rhs = fnStack.top(); fnStack.pop();
				Function lhs = fnStack.top(); fnStack.pop();
				fnStack.push([&lhs, &rhs](ld x) {return lhs(x) * rhs(x); });
			}
			else if (token == "/") {
				if (parsed.size() < 2)
					throw std::runtime_error("Expression parsing failed");
				Function rhs = fnStack.top(); fnStack.pop();
				Function lhs = fnStack.top(); fnStack.pop();
				fnStack.push([&lhs, &rhs](ld x) {return lhs(x) / rhs(x); });
			}
			else if (token == "^") {
				if (parsed.size() < 2)
					throw std::runtime_error("Expression parsing failed");
				Function rhs = fnStack.top(); fnStack.pop();
				Function lhs = fnStack.top(); fnStack.pop();
				fnStack.push([&lhs, &rhs](ld x) {return std::pow(lhs(x), rhs(x)); });
			}
			else if (token.back() == '\'') {
				if (parsed.size()) {
					Function arg = fnStack.top(); fnStack.pop();
					Function fn;
					std::string _identifier = token.substr(0, token.size() - 1);
					if (_identifier.size() == 1 and _identifier[0] >= identifier)
						throw std::invalid_argument("User-defined function calls must be to preceeding or builtin functions");
					auto tmp = builtInFunctions.find(_identifier);
					if (tmp == builtInFunctions.end()) {
						fn = tmp->second;
					}
					else {
						tmp = functions.find(_identifier);
						if (tmp == functions.end())
							throw std::invalid_argument("Invalid function identifier");
						fn = tmp->second;
					}

					Function fn_prime = derivative(fn);

					fnStack.push([&fn_prime, &arg](ld x) {return fn_prime(arg(x)); });
				}
				else
					throw std::runtime_error("Illegal derivative");
			}
			else if (std::all_of(token.begin(), token.end(), ::isalpha)) {
				Function arg = fnStack.top(); fnStack.pop();
				Function fn;
				if (token.size() == 1 and token[0] >= identifier)
					throw std::invalid_argument("User-defined function calls must be to preceeding or builtin functions");
				auto tmp = builtInFunctions.find(token);
				if (tmp == builtInFunctions.end()) {
					fn = tmp->second;
				}
				else {
					tmp = functions.find(token);
					if (tmp == functions.end())
						throw std::invalid_argument("Invalid function identifier");
					fn = tmp->second;
				}
				fnStack.push([&fn](ld x) {return fn(x); });
			}

		}
	};
	functionMapping functions;
	functionMapping builtInFunctions;
	std::queue<std::string> parsed;
	std::vector<std::string> tokens;
public:
	FunctionFactory() = default;
	FunctionFactory(functionMapping& fns) {
		functions = fns;
	}
	~FunctionFactory() = default;
	functionMapping& getFunctions() {
		return functions;
	};
	void parseFunction(std::string& expression, char identifier);
};