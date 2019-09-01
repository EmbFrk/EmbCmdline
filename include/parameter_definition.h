/******************************
* definition of one parameter *
******************************/

#ifndef _PARAMETER_DEFINITION_H
	#define _PARAMETER_DEFINITION_H

	/* this structure is required because it is used */
	/* as one parameter of the parameter function    */
	struct one_command;

	struct one_par_def {
		/* short definition of the parameter */
		char *par_short;

		/* long definition of one parameter */
		char *par_long;

		/* help for this parameter */
		char *par_help;

		/* function to call if this parameter was found */
		int (*par_fkt)(const struct one_command *called, const char *parameter, int validx);

		/* character to use for separation */
		/* of parameter and its value      */
		char par_sep;
	};
#endif
