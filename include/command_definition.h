#ifndef _COMMAND_DEFINITION_H
	#define _COMMAND_DEFINITION_H

	// for NULL usage
	#include <stddef.h>

	#include <parameter_definition.h>

	typedef int (*onecmd_fkt)(int argc, const char *argv[]);

	struct one_command {
		const char *name;
		const int argc_min;
		const int argc_max;
		const char *help_short;
		const char *help;
		const onecmd_fkt fkt;
		const int parameter_amount;
		const struct one_par_def **parameter;
	};
#endif
