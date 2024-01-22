#ifndef COLOR_HPP
# define COLOR_HPP
# include "common.hpp"

namespace color {
    const std::string RESET = "\033[0m";
    const std::string BLACK = "\033[1;30;49m";
    const std::string RED = "\033[1;31;49m";
    const std::string GREEN = "\033[1;32;49m";
    const std::string YELLOW = "\033[1;33;49m";
    const std::string BLUE = "\033[1;34;49m";
    const std::string MAGENTA = "\033[1;35;49m";
    const std::string CYAN = "\033[1;36;49m";
    const std::string WHITE = "\033[1;37;49m";
	const std::string BG_GREY = "\033[37;40m";
	const std::string BG_RED = "\033[1;30;41m";
	const std::string BG_GREEN = "\033[1;30;42m";
	const std::string BG_YELLOW = "\033[30;43m";
	const std::string BG_BLUE = "\033[1;30;44m";
	const std::string BG_MAGENTA = "\033[1;30;45m";
	const std::string BG_CYAN = "\033[30;46m";
	const std::string BG_WHITE = "\033[30;47m";
	const std::string BG_DEFAULT = "\033[49m";
	const std::string BG_YELLOW_CLR_CYAN = "\033[36;43m";
	const std::string BG_YELLOW_CLR_MAGENTA = "\033[35;43m";
	const std::string BG_YELLOW_CLR_BLUE = "\033[34;43m";

	inline std::string redText(const std::string &str) { return RED + str + RESET; }
	inline std::string greenText(const std::string &str) { return GREEN + str + RESET; }
	inline std::string cyanText(const std::string &str) { return CYAN + str + RESET; }
	inline std::string yellowText(const std::string &str) { return YELLOW + str + RESET; }
}
#endif