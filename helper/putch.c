#if LINUX_PUTCH
#include <stdio.h>

void putch(char c)
{
	printf("%c", c);
}
#endif
