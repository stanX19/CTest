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
std::string utils::reduceStringTo(const std::string& input, std::size_t length) {
    if (length >= input.length()) {
        return input;
    } else {
        std::size_t suffixLength = (length - 3) / 2;
        std::size_t prefixLength = length - 3 - suffixLength;
        return input.substr(0, prefixLength) + "..." + input.substr(input.length() - suffixLength);
    }
}
void utils::fillLine(std::stringstream &ss, const char fillChar, std::size_t length) {
	std::string existingText = ss.str();
	size_t lineBreak = existingText.rfind('\n');
	length -= existingText.length() - (lineBreak + 1) * (lineBreak != existingText.npos);
    ss << std::setfill(fillChar);
    ss << std::setw(length) << "" << std::endl;
    ss << std::setfill(' ');
}