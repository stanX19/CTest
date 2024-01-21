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
std::string utils::generateRandomString(std::size_t length) {
	static int idx;
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const size_t charsetSize = sizeof(charset) - 1;  // Exclude the null terminator

    std::string randomString;

    for (size_t i = 0; i < length; ++i) {
        randomString += charset[idx % charsetSize];
		idx = ++idx % charsetSize;
    }

    return randomString;
}
std::string utils::formatUnprintable(const std::string &str) {
    std::ostringstream oss;
    
    for (char ch : str) {
        if (ch < 32 || ch > 126) {
            oss << "^" << static_cast<char>(ch + 64);
        } else if (ch == 127) {
            oss << "^?";
        } else {
            oss << ch;
        }
    }
    
    return oss.str();
}
