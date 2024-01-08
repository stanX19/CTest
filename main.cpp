#include "UnitTest.hpp"

int main() {
	UnitTest test("ex00");
	test.addRequiredFile("ft_putchar.c");
	test.addTempCodeFile("ft_putchar(argv[1]);");
	test.addTestCase("a", "a");
	test.addTestCase("x", "b");
	test.addTestCase("c", "c");
	test.run();
}
