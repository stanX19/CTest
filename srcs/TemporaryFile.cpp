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
	//std::cout << "deleted " << filename_ << "\n"; 
}

void TemporaryFile::setContent(const std::string& content) {
	//std::cout << "modified " << filename_ << "\n";
	std::ofstream file(filename_);
	file << content;
	file.close();
}

void TemporaryFile::setFilename(const std::string& name) {
	//std::cout << "rename " << filename_ << " " << name << "\n";
	if (std::rename(filename_.c_str(), name.c_str()) != 0)
	{
		std::cerr << "Error renaming temporary file '" << filename_ << "' to '" << name << "'" << std::endl;
	}
	filename_ = name;
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
	setContent("");
}
