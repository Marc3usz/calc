#include "cli.hpp"

#include <iostream>

CliHandler::CliHandler(int argc, char** argv): options("calc", "graphical calculator with SDL2") {
	options.add_options()
		("file", "file to open functions from", cxxopts::value<std::string>())
		("font", "specify path to font file (ttf)", cxxopts::value<std::string>()->default_value("C:\\Windows\\Fonts\\arial.ttf"))
		("w,width", "specify width, default 800", cxxopts::value<int>()->default_value("800"))
		("h,height", "specify height, default 600", cxxopts::value<int>()->default_value("600"))
		;
	parsed = options.parse(argc, argv);
}

std::string CliHandler::fontFilePath() {
	return parsed["font"].as<std::string>();
}

std::optional<std::string> CliHandler::loadPath() {
	if (parsed.count("file")) {
		return parsed["file"].as<std::string>();
	}
	else {
		return std::optional<std::string>();
	}
}

int CliHandler::width() {
	if (!parsed.count("width"))
	{
		return parsed["w"].as<int>();
	}
	else {
		return parsed["width"].as<int>();
	}
}

int CliHandler::height() {
	if (!parsed.count("height"))
	{
		return parsed["h"].as<int>();
	}
	else {
		return parsed["height"].as<int>();
	}
}