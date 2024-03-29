/***************************************
* implementation of the command 'help' *
***************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <putch.h>
#include "command_definition.h"
#include "all_commands.h"

static int do_help(int argc, const char *argv[]);
static char const help_name[] =	"help";
static char const help_help[] =	"prints help for commands\r\n" \
				"accepts a command as parameter\r\n" \
				"for printing detailed help for this command";
static char const help_sh[] =	"print help";

const struct one_command cmd_help = {
	.name = help_name,
	.argc_min = 1,
	.argc_max = 2,
	.help_short = help_sh,
	.help = help_help,
	.fkt = do_help
};

/* this command actually doesn't use the parameter mechanism */
/* wouldn't be a big difference, just an additional function */
int do_help(int argc, const char *argv[])
{
	int i,j, h;
	int ls, ll;
	char maxlen;
	char c;

	maxlen = 0;

	if(argc == 1) {
		/* no parameter                     */
		/* --> print all short help strings */
		printf("list of all commands:\r\n");
		printf("use \"help <command>\" to get detailed help\r\n");

		/* first search the maximum length of all commands */
		for(i =  0; i < all_commands_entries; i++) {
			if(all_commands[i]->help_short != NULL) {
				j = strlen(all_commands[i]->name) + 1;
				if(j > maxlen) {
					maxlen = j;
				}
			}
		}
		maxlen++;

		/* then print them all */
		for(i =  0; i < all_commands_entries; i++) {
			if(all_commands[i]->help_short != NULL) {
				j = printf("%s", all_commands[i]->name);
				while(j < maxlen) {
					putch(' ');
					j++;
				}
				printf("%s\r\n", all_commands[i]->help_short);
			}
		}
	} else if(argc == 2) {
		/* parameter is the command which help shall be printed */

		/* search the command */
		for(i =  0; i < all_commands_entries; i++) {
			c = strlen(argv[1]);
			j = strlen(all_commands[i]->name);
			if(j != c) {
				continue;
			}
			if(strstr(all_commands[i]->name, argv[1]) == all_commands[i]->name) {
				break;
			}
		}

		if(i < all_commands_entries) {
			/* command found */
			printf("\"%s\" - %s\r\n",
					all_commands[i]->name,
					all_commands[i]->help);
			ls = 0;
			ll = 0;

			/* calc longes short parameter */
			/* and longest long paramerer  */
			for(j = 0; j < all_commands[i]->parameter_amount; j++) {
				if(all_commands[i]->parameter[j]->par_short != NULL) {
					h = strlen(all_commands[i]->parameter[j]->par_short);
					if(h > ls) {
						ls = h;
					}
				}
				if(all_commands[i]->parameter[j]->par_long != NULL) {
					h = strlen(all_commands[i]->parameter[j]->par_long);
					if(h > ll) {
						ll = h;
					}
				}
			}

			/* then print the complete help */
			for(j = 0; j < all_commands[i]->parameter_amount; j++) {
				h = 0;
				if(ls) {
					if(all_commands[i]->parameter[j]->par_short != NULL) {
						h = printf("%s", all_commands[i]->parameter[j]->par_short);
					}
				}
				while(h < (ls + 2)) {
					putch(' ');
					h++;
				}
				h = 0;
				if(ll) {
					if(all_commands[i]->parameter[j]->par_long != NULL) {
						h = printf("%s  ", all_commands[i]->parameter[j]->par_long);
					}
				}
				while(h < (ll + 2)) {
					putch(' ');
					h++;
				}
				printf(" %s\r\n", all_commands[i]->parameter[j]->par_help);
			}
		} else {
			printf("%s: unknown command %s\r\n", argv[0], argv[1]);
		}
	}

	return(0);
}
