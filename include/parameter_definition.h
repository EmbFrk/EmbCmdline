#ifndef _PARAMETER_DEFINITION_H
	#define _PARAMETER_DEFINITION_H

	struct one_command;

	struct one_par_def {
		char *par_short;
		char *par_long;
		char *par_help;
		int (*par_fkt)(const struct one_command *called, const char *parameter, int validx);
		char par_sep;
	};
#endif
