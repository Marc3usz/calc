#pragma once

#include <string>
#include <vector>

namespace fileHandler {
	std::vector<std::string> loadFunctions(std::string);
	void saveFile(std::vector<std::string>, std::string);
};