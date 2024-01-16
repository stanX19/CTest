#include "TemporaryFile.hpp"

TemporaryFile::TemporaryFile(const std::string& content, const std::string& type) : type_(type) {
	generateUniqueFilename();
	setContent(content);
}

TemporaryFile::TemporaryFile(const TemporaryFile &other) : type_(other.type_) {
	generateUniqueFilename();
	setContent(other.readContent());
}

TemporaryFile::~TemporaryFile() {
	if (std::remove(filename_.c_str()) != 0) {
		std::cerr << "Error deleting temporary file '" << filename_ << "'" << std::endl;
	}
}

void TemporaryFile::setContent(const std::string& content) {
	std::ofstream file(filename_);
	file << content;
	file.close();
}

std::string TemporaryFile::filename() const {
	return filename_;
}

std::string TemporaryFile::readContent() const {
	std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Error opening file '" << filename_ << "' for reading." << std::endl;
		return "";
	}
	std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void TemporaryFile::generateUniqueFilename() {
	int counter = 1;
	do {
		std::ostringstream oss;
		oss << "temp" << std::setfill('0') << std::setw(3) << counter << type_;
		filename_ = oss.str();
		counter++;
	} while (utils::pathExists(filename_));
}
