#include "UnitTest.hpp"

int main() {
	UnitTest test("ex00");
	test.addRequiredFile("ft_putchar.c");
	test.addTemporaryMainFile(
		"void ft_putchar(char c);",
		"ft_putchar(argv[1][0]);"
	);
	test.addTestCase("a", "a");
	test.addTestCase("x", "x");
	test.addTestCase("c", "c");
	test.run();
}
