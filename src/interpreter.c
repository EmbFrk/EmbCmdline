#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <interpreter.h>
#include "command_definition.h"
#include "is_whitespace.h"
#include "all_commands.h"

static const char *argv[MAX_ARGC];

static int parameter_handling(const struct one_command *cmd, int argc, const char *argv[]);
static int handle_one_par(const struct one_command *cmd, const char *par);

int interpreter(char *cmdline, int length)
{
	int ret = 1;
	int cmdlen;
	int arg_idx;
	int i, j;
	bool found = false;

	arg_idx = 0;
	i = 0;

	do {
		argv[arg_idx] = NULL;
		while(i < length) {
			if(!is_whitespace(cmdline[i])) {
				break;
			}
			i++;
		}
		if(i >= length) {
			break;
		}
		if(cmdline[i] == '\0') {
			break;
		}
		j = i;
		while(j < length) {
			if(cmdline[j] == '\0') {
				break;
			}
			if(is_whitespace(cmdline[j])) {
				break;
			}
			j++;
		}
		if(i != j) {
			argv[arg_idx] = &cmdline[i];
			if(arg_idx == 0) {
				cmdlen = j - i + 1;
			}
			arg_idx++;
			if(cmdline[j] == '\0') {
				break;
			} else {
				cmdline[j] = '\0';
			}
			i = j + 1;
		} else {
			break;
		}
	} while(i < length);

	do {
		if(argv[0] == NULL) {
			break;
		}

		i = 0;
		j = -1;
		cmdlen = strlen(argv[0]) + 1;

		while(i < all_commands_entries) {
			if(strstr(all_commands[i]->name, argv[0]) == all_commands[i]->name) {
				if(j < 0) {
					j = i;
					found = true;
				} else {
					printf("command not unique\r\n");
					found = false;
					j = -2;
					break;
				}
				if(all_commands[i]->name[cmdlen] == '\0') {
					found = true;
					j = i;
					break;
				}
				if(all_commands[i]->help_short == NULL) {
					found = false;
					j = -1;
					break;
				}
			}
			i++;
		}

		if(!found) {
			if(j != -2) {
				printf("\aunknown command %s\r\n", argv[0]);
			}
			break;
		}

		if(j < 0) {
			break;
		}

		ret = parameter_handling(all_commands[j], arg_idx, argv);
		if(ret) {
			break;
		}

		ret = all_commands[j]->fkt(arg_idx, argv);
	} while(false);

	return(ret);
}

int parameter_handling(const struct one_command *cmd, int argc, const char *argv[])
{
	int ret = 1;	// error
	int i;

	do {
		if(cmd->argc_min && (argc < cmd->argc_min)) {
			printf("%s: not enough parameters\r\n", argv[0]);
			break;
		}

		if(cmd->argc_max && (argc > cmd->argc_max)) {
			printf("%s: to many parameters\r\n", argv[0]);
			break;
		}

		if(cmd->parameter == NULL) {
			/* no parameter definition */
			ret = 0;
			break;
		}

		if(argc == 1) {
			/* no parameter given --> all good */
			ret = 0;
			break;
		}

		for(i = 1; i < argc; i++) {
			ret = handle_one_par(cmd, argv[i]);
			if(ret) {
				break;
			}
		}

		if(ret) {
			break;
		}
		ret = 0;
	} while(false);

	return(ret);
}

static int check(const char *par, const char val)
{
	int ret = 1;

	if(par[0] == val) {
		/* parameter end reached */
		if(val == '\0') {
			/* parameter without value, all fine */
			ret = 0;
		} else {
			if(par[1] == '\0') {
				/* there is no value */
				ret = 2;
			} else {
				/* value found, all fine */
				ret = 0;
			}
		}
	} else {
		if(val != '\0') {
			ret = 2;
		}
	}

	return(ret);
}

static void print(const struct one_command *cmd, int error, const char *par)
{
	switch(error) {
		case 1:
			printf("%s: unknown parameter %s\r\n", cmd->name, par);
			break;
		case 2:
			printf("%s: parameter %s requires a value\r\n", cmd->name, par);
			break;
		default:
			break;
	}
}

int handle_one_par(const struct one_command *cmd, const char *par) {
	int ret = 1;	// error
	int i;
	int parlen;
	const struct one_par_def *pardef;

	do {
		for(i = 0; i < cmd->parameter_amount; i++) {
			pardef = cmd->parameter[i];
			if((pardef->par_long == NULL) && (pardef->par_short == NULL)) {
				parlen = 0;
				ret = 0;
				/* default fallback */
				break;
			}
			parlen = strlen(pardef->par_short);
			if(!strncmp(pardef->par_short, par, parlen)) {
				ret = check(&par[parlen], pardef->par_sep);
				if(ret) {
					print(cmd, ret, par);
				}
				break;
			}
			parlen = strlen(pardef->par_long);
			if(!strncmp(pardef->par_long, par, parlen)) {
				ret = check(&par[parlen], pardef->par_sep);
				if(ret) {
					print(cmd, ret, par);
				}
				break;
			}
		}

		if(i >= cmd->parameter_amount) {
			print(cmd, 1, par);
			break;
		}

		if(ret) {
			break;
		}

		if(i < cmd->parameter_amount) {
			if(pardef->par_sep != '\0') {
				parlen++;
			}
			ret = pardef->par_fkt(cmd, par, parlen);
		}
	} while(false);

	return(ret);
}
