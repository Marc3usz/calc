#pragma once

#include <cxxopts.hpp>
#include <string>
#include <optional>

class CliHandler {
private:
	cxxopts::Options options;
	cxxopts::ParseResult parsed;
public:
	CliHandler(int argc, char** argv);
	~CliHandler() = default;
	std::string fontFilePath();
	std::optional<std::string> loadPath();
	int width();
	int height();
};