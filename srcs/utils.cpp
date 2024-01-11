#include "utils.hpp"

// without dot
std::string utils::getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.rfind('.');
    if (dotPos < filePath.length() - 1) {
        return filePath.substr(dotPos + 1);
    }
    return "";
}

bool utils::pathExists(const std::string& filePath) {
    return std::ifstream(filePath).good();
}

void utils::printOK(const std::string &dir) {
	std::cout << color::greenText(dir + ": OK") << std::endl;
}
void utils::printKO(const std::string &dir) {
	std::cout << color::redText(dir + ": KO") << std::endl;
}