#include "all_commands.h"

extern struct one_command cmd_help;
extern struct one_command cmd_hidden;
extern struct one_command cmd_parameter;
extern struct one_command cmd_version;
extern struct one_command cmd_exit;

const struct one_command * const all_commands[] = {
	&cmd_help,
	&cmd_hidden,
	&cmd_parameter,
	&cmd_version,
	&cmd_exit,
};

int const all_commands_entries = sizeof(all_commands) / sizeof(all_commands[0]);
