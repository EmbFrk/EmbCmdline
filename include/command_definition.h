/**********************************
* This is how to define a command *
**********************************/

#ifndef _COMMAND_DEFINITION_H
	#define _COMMAND_DEFINITION_H

	// for NULL usage
	#include <stddef.h>

	#include <parameter_definition.h>

	/* maximum of parameters for any command */
	#define MAX_ARGC	10

	/* prototype for one command function */
	typedef int (*onecmd_fkt)(int argc, const char *argv[]);

	/* structure for command definition */
	struct one_command {
		/* pointer to full name of the command */
		const char *name;

		/* argc has to be at least */
		const int argc_min;

		/* argc has to be lower or equal to */
		const int argc_max;

		/* short help for the command                  */
		/* If this is NULL the command is 'hidden'     */
		/* that means: it has to be completely entered */
		/* no shortcut possible                        */
		const char *help_short;

		/* detailed help for the command */
		const char *help;

		/* pointer to the commands function */
		const onecmd_fkt fkt;

		/* amount of defined parameters */
		/* this is NOT the maximum of parameters to give! */
		/* this is number of elements of the array where  */
		/* 'parameter' points to!                         */
		const int parameter_amount;

		/* pointer to the array of parameter */
		/* definition for this command       */
		const struct one_par_def **parameter;
	};
#endif
