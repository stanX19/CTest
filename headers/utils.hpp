#ifndef UTILS_HPP
# define UTILS_HPP
# include "common.hpp"
# include "color.hpp"
namespace utils {
	std::string getFileExtension(const std::string& filePath);
	void printOK(const std::string &dir);
	void printKO(const std::string &dir);
}
#endif