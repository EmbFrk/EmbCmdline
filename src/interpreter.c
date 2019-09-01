/*********************************
* This is the interpreter itself *
*********************************/

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

/***************************************
* interpreter                          *
* in:  pointer to complete commandline *
*      length of the commandline       *
* out: == 0 --> all fine               *
*      != 0 --> error                  *
***************************************/
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
		/* divide the cmdline into argv strings */
		argv[arg_idx] = NULL;

		/* as long as there are chars in the cmdline */
		while(i < length) {
			/* ignore whitespaces */
			if(!is_whitespace(cmdline[i])) {
				break;
			}
			i++;
		}

		/* keep care not to leave the cmdline */
		if(i >= length) {
			break;
		}

		/* stop on end of commandline */
		if(cmdline[i] == '\0') {
			break;
		}

		/* now search for the end of one argv */
		j = i;
		while(j < length) {
			/* stop on end of line */
			if(cmdline[j] == '\0') {
				break;
			}
			/* stop on end of this argv */
			if(is_whitespace(cmdline[j])) {
				break;
			}
			j++;
		}

		if(i != j) {
			/* found one argv */
			/* add it to the array */
			argv[arg_idx] = &cmdline[i];
			if(arg_idx == 0) {
				/* remember the length of the command */
				cmdlen = j - i + 1;
			}
			arg_idx++;

			if(cmdline[j] == '\0') {
				/* stop on commandline end */
				break;
			} else {
				/* mark end of argv */
				cmdline[j] = '\0';
			}
			/* calculate the next start index */
			i = j + 1;
		} else {
			break;
		}
	} while(i < length);

	do {
		if(argv[0] == NULL) {
			/* no command found --> exit */
			break;
		}

		i = 0;
		j = -1;

		/* search the command */
		while(i < all_commands_entries) {
			/* walk through all available commands */

			if(strstr(all_commands[i]->name, argv[0]) == all_commands[i]->name) {
				/* if received command fits to the */
				/* command with the actual index   */
				if(j < 0) {
					/* we found the first ocurance */
					j = i;
					found = true;
				} else {
					/* this is not the first ocurence */
					printf("command not unique\r\n");
					found = false;
					j = -2;
					break;
				}
				if(all_commands[i]->name[cmdlen] == '\0') {
					/* end of known command reached */
					/* --> found it                 */
					found = true;
					j = i;
					break;
				}
				if(all_commands[i]->help_short == NULL) {
					/* the 'hidden command' feature     */
					/* end of known command not reached */
					/* but command is hidden (no short  */
					/* help) --> command not found      */
					found = false;
					j = -1;
					break;
				}
			}
			i++;
		}

		if(!found) {
			if(j != -2) {
				/* -2 would be 'not unique' */
				printf("\aunknown command %s\r\n", argv[0]);
			}
			break;
		}

		/* valid commandfound, act_idx points to it */

		/* first handle the parameter */
		ret = parameter_handling(all_commands[j], arg_idx, argv);
		if(ret) {
			/* stop in case of parameter error */
			break;
		}

		/* last: call the function of the command */
		ret = all_commands[j]->fkt(arg_idx, argv);
		/* return value is returned to the caller */
	} while(false);

	return(ret);
}

/*******************************************************
* generic parameter handling                           *
* in:  pointer to the command definition               *
*      argc                                            *
*      argv[]                                          *
* out: return value of the parameter handling function *
*      == 0 --> no error                               *
*      != 0 --> error                                  *
*******************************************************/
int parameter_handling(const struct one_command *cmd, int argc, const char *argv[])
{
	int ret = 1;	// error
	int i;

	do {
		/* check against minimum argument amount */
		if(cmd->argc_min && (argc < cmd->argc_min)) {
			printf("%s: not enough parameters\r\n", cmd->name);
			break;
		}

		/* check against maximum argument amount */
		if(cmd->argc_max && (argc > cmd->argc_max)) {
			printf("%s: to many parameters\r\n", cmd->name);
			break;
		}

		/* check if spcific argument definition is given */
		/* if not: no parameter error, parameter are     */
		/* handled inside the command function           */
		if(cmd->parameter == NULL) {
			/* no parameter definition */
			ret = 0;
			break;
		}

		/* command without parameter: everything fine */
		if(argc == 1) {
			ret = 0;
			break;
		}

		/* handle each parameter */
		for(i = 1; i < argc; i++) {
			ret = handle_one_par(cmd, argv[i]);
			if(ret) {
				/* report first parameter error */
				break;
			}
		}
	} while(false);

	return(ret);
}

/*****************************************************
* checks if a parameter value is required            *
* and if yes: if it exists                           *
* in:  pointer to first char behind parameter value  *
*      separator value                               *
* out: == 0 --> parameter fits to format requirement *
*      != 0 --> parameter error                      *
*****************************************************/
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

static void print_error(const struct one_command *cmd, int error, const char *par)
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

/*************************************
* handle one parameter               *
* in:  pointer to command definition *
*      pointer to received parameter *
* out: == 0 --> no error             *
*      != 0 --> error                *
*************************************/
int handle_one_par(const struct one_command *cmd, const char *par) {
	int ret = 1;	// error
	int i;
	int parlen;
	const struct one_par_def *pardef;

	do {
		/* walk trough all parameters of this command */
		for(i = 0; i < cmd->parameter_amount; i++) {
			/* get defined parameter */
			pardef = cmd->parameter[i];
			/* if no parameter defined:      */
			/* fall through - can be used    */
			/* for parameters without prefix */
			if((pardef->par_long == NULL) && (pardef->par_short == NULL)) {
				parlen = 0;
				ret = 0;
				break;
			}
			/* check for short parameter */
			parlen = strlen(pardef->par_short);
			if(
				(pardef->par_short != NULL) &&
				!strncmp(pardef->par_short, par, parlen)
			) {
				/* match! --> check for completed */
				/* and parameter value            */
				ret = check(&par[parlen], pardef->par_sep);
				if(ret) {
					print_error(cmd, ret, par);
				}
				break;
			}
			/* check for short parameter */
			parlen = strlen(pardef->par_long);
			if(
				(pardef->par_long != NULL) &&
				!strncmp(pardef->par_long, par, parlen)
			) {
				/* match! --> check for completed */
				/* and parameter value            */
				ret = check(&par[parlen], pardef->par_sep);
				if(ret) {
					print_error(cmd, ret, par);
				}
				break;
			}
		}

		/* check if parameter found */
		if(i >= cmd->parameter_amount) {
			/* parameter NOT found --> error */
			print_error(cmd, 1, par);
			break;
		}

		/* check for any error */
		if(ret) {
			break;
		}

		/* everything fine */
		if(pardef->par_sep != '\0') {
			/* increment pointer to parameter value      */
			/* --> handover index to first char of value */
			parlen++;
		}
		/* call specific parameter handling function */
		ret = pardef->par_fkt(cmd, par, parlen);
	} while(false);

	return(ret);
}
