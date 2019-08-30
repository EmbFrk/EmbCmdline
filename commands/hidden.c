#include <stdio.h>

#include <command_definition.h>

int do_hidden(int argc, const char *argv[]);

static char const hidden_name[] = "hidden";
static char const hidden_help[] = "hidden help long";

const struct one_command cmd_hidden = {
	.name = hidden_name,
	.help_short = NULL,
	.help = hidden_help,
	.fkt = do_hidden
};

int do_hidden(int argc, const char *argv[])
{
	printf("This is the result of command %s\r\n", argv[0]);

	return(0);
}
