#ifndef COLOR_HPP
# define COLOR_HPP
# include "common.hpp"

namespace color {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[1;31m";
    const std::string GREEN = "\033[1;32m";
    const std::string YELLOW = "\033[1;33m";
    const std::string BLUE = "\033[1;34m";
    const std::string MAGENTA = "\033[1;35m";
    const std::string CYAN = "\033[1;36m";
    const std::string WHITE = "\033[1;37m";

	inline std::string redText(const std::string &str) { return RED + str + RESET; }
	inline std::string greenText(const std::string &str) { return GREEN + str + RESET; }
	inline std::string cyanText(const std::string &str) { return CYAN + str + RESET; }
	inline std::string yellowText(const std::string &str) { return YELLOW + str + RESET; }
}
#endif