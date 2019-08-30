#include "is_whitespace.h"

int is_whitespace(char c)
{
	int ret = 0;
	int i;
	static const char seps[] = " \t\r\n";

	for(i = 0; i < sizeof(seps) - 1; i++) {
		if(c == seps[i]) {
			ret = 1;
			break;
		}
	}

	return(ret);
}
