#include "UnitTest.hpp"

const std::string MAIN_START = "int main(int argc, char **argv)\n{(void)argc;(void)argv;";
const std::string MAIN_END = "\n}\n";

int main() {
	UnitTest test("ex00");
	test.addRequiredFile("ft_putchar.c");
	test.addTempCodeFile("void ft_putchar(char c);\n" + MAIN_START + "ft_putchar(argv[1][0]);" + MAIN_END);
	test.addTestCase("a", "a");
	test.addTestCase("x", "x");
	test.addTestCase("c", "c");
	test.run();
}
