#pragma once

#include <string>
#include <vector>

namespace fileHandler {
	std::vector<std::string> loadFunctions(std::string);
	void saveFunctions(std::vector<std::string>, std::string);
	void saveRoots(std::vector<long double>, std::string);
};