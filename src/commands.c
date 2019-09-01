/*********************************************
* define all commands to use in this file    *
* idea behind:                               *
* use 'extern struct one_command <name>'     *
* and then add the pointer to all_commands[] *
*********************************************/

#include "all_commands.h"

extern struct one_command cmd_help;
extern struct one_command cmd_hidden;
extern struct one_command cmd_parameter;
extern struct one_command cmd_version;
extern struct one_command cmd_exit;

/* this is the central point to */
/* define all existing commands */
const struct one_command * const all_commands[] = {
	&cmd_help,
	&cmd_hidden,
	&cmd_parameter,
	&cmd_version,
	&cmd_exit,
};

/* DO NOT CHANGE THIS LINE! */
int const all_commands_entries = sizeof(all_commands) / sizeof(all_commands[0]);
