#ifndef UTILS_HPP
# define UTILS_HPP
# include "common.hpp"
# include "color.hpp"
namespace utils {
	std::string getFileExtension(const std::string& pat);
	bool pathExists(const std::string &path);
	void printOK(const std::string &dir, const std::string &end="\n");
	void printKO(const std::string &dir, const std::string &end="\n");
	std::string reduceStringTo(const std::string& input, std::size_t length);
	void fillLine(std::stringstream &ss, const char fillChar, std::size_t length);
	std::string generateRandomString(std::size_t length);
	std::string formatUnprintable(const std::string &str);
}
#endif