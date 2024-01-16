#ifndef UTILS_HPP
# define UTILS_HPP
# include "common.hpp"
# include "color.hpp"
namespace utils {
	class WorkAt {
	public:
		WorkAt(std::string &path) {
			originalPath_ = std::filesystem::current_path();
			std::filesystem::current_path(path);
		}
		~WorkAt() {
			std::filesystem::current_path(originalPath_);
		}
	private:
		std::string originalPath_;
	};
	std::string getFileExtension(const std::string& pat);
	bool pathExists(const std::string &path);
	void printOK(const std::string &dir, const std::string &end="\n");
	void printKO(const std::string &dir, const std::string &end="\n");
}
#endif