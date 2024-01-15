#ifndef UTILS_HPP
# define UTILS_HPP
# include "common.hpp"
# include "color.hpp"
namespace utils {
	std::string getFileExtension(const std::string& pat);
	bool dirPathExists(const std::string &dirPath);
	bool filePathExists(const std::string &filePath);
	void printOK(const std::string &dir, const std::string &end="\n");
	void printKO(const std::string &dir, const std::string &end="\n");
}
#endif