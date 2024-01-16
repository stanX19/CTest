#include "utils.hpp"

// without dot
std::string utils::getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.rfind('.');
    if (dotPos < filePath.length() - 1) {
        return filePath.substr(dotPos + 1);
    }
    return "";
}

bool utils::pathExists(const std::string& path) {
	return std::filesystem::exists(path);
}

void utils::printOK(const std::string &dir, const std::string &end) {
	std::cout << color::greenText(dir + ": OK") << end;
}
void utils::printKO(const std::string &dir, const std::string &end) {
	std::cout << color::redText(dir + ": KO") << end;
}