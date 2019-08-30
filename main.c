#include <string.h>

#include <lineedit.h>
#include <interpreter.h>

int main(int argc, char *argv[])
{
	char *line;

	do {
		line = lineedit("EmbCmdline$ ");
		if(line != NULL) {
			interpreter(line, strlen(line) + 1);
		}
	} while(!leave_interpreter);
}
