/****************************************************
* handling for whitespace                           *
* can be 'misused' to tag other chars as whitespace *
****************************************************/

#include "is_whitespace.h"

bool is_whitespace(char c)
{
	bool ret = false;
	int i;
	static const char seps[] = " \t\r\n";

	for(i = 0; i < sizeof(seps) - 1; i++) {
		if(c == seps[i]) {
			ret = true;
			break;
		}
	}

	return(ret);
}
