#include "fileHandler.hpp"

#include <fstream>
#include <iostream>

std::vector<std::string> fileHandler::loadFunctions(std::string path) {
    std::vector<std::string> res;

    std::ifstream testFile(path);
    if (!testFile.good()) {
        std::cerr << "Error: Cannot open file at path: " << path << std::endl;
        return res;
    }
    testFile.close();

    try {
        std::ifstream fFile(path, std::ios::in);
        std::string line;

        while (std::getline(fFile, line)) {
            if (!line.empty()) {
                res.push_back(line);
            }
        }

        fFile.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error reading file" << std::endl;
    }

    return res;
}

void fileHandler::saveFile(std::vector<std::string> functions, std::string path) {
	std::ofstream fFile(path, std::ios::out);
	for (std::string function : functions) {
		fFile << function << "\n";
	}
	fFile.close();
}
