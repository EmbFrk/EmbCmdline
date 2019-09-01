/**************************
* How to use this project *
**************************/

#include <string.h>

#include <lineedit.h>
#include <interpreter.h>

#include <command_definition.h>

int leave_interpreter = 0;

int main(int argc, char *argv[])
{
	int ret = 1;
	char *line;

	/* rather simple: */
	do {
		/* call the line editor */
		line = lineedit("EmbCmdline$ ");
		if(line != NULL) {
			/* and use its output for the interpreter */
			ret = interpreter(line, strlen(line) + 1);
		}
		/* until someone sets 'leave_interpreter' */
	} while(!leave_interpreter);

	return(ret);
}

/* implementation of the command 'exit' */
static char const exit_name[] =	"exit";
static int exit_fkt(int argc, const char *argv[]);
static char exit_help[] = "exit the interpreter";

const struct one_command cmd_exit = {
	.name = exit_name,
	.help_short = exit_help,
	.help = exit_help,
	.fkt = exit_fkt,
	.parameter = NULL,
};

int exit_fkt(int argc, const char *argv[])
{
	leave_interpreter = 1;

	return(0);
}
