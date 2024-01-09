#include <unistd.h>

void	ft_putchar(char c)
{
	char *x = "abc";
	x[1] = 'x';
	write(1, &c, 1);
}