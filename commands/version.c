#include <stdio.h>

#include <command_definition.h>

#include "version.h"

int do_version(int argc, const char *argv[]);

static char const vers_name[] =	"version";
static char const vers_help[] =	"print version information";
static char const vers_sh[] =	"print version";

const struct one_command cmd_version = {
	.name = vers_name,
	.help_short = vers_sh,
	.help = vers_help,
	.fkt = do_version
};

int do_version(int argc, const char *argv[])
{
	printf("%s\r\n", base_version);

	return(0);
}
