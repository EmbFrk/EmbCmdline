/******************************************
* detailed example for a complex function *
******************************************/

#include <stdio.h>

#include <command_definition.h>

static int do_parameter(int argc, const char *argv[]);
static int found_p1(const struct one_command *called, const char *parameter, int validx);
static int found_p2(const struct one_command *called, const char *parameter, int validx);
static int found_default(const struct one_command *called, const char *parameter, int validx);

/* definition of parameter '-p1'   */
/* this parameter expects no value */
/* --> par_sep = '\0'              */
static const struct one_par_def par1 = {
	.par_short = "-p1",
	.par_long = "--parameter1",
	.par_help = "this is the help for parameter1",
	.par_fkt = found_p1,
	.par_sep = '\0'
};

/* definition of parameter '-p2'  */
/* this parameter expects a value */
/* par_sep = '='                  */
/* --> usage: -p2=xyz             */
static const struct one_par_def par2 = {
	.par_short = "-p2",
	.par_long = "--parameter2",
	.par_help = "this is the help for parameter2",
	.par_fkt = found_p2,
	.par_sep = '='
};

/* this is the 'fall through' parameter  */
/* this is called in any case which does */
/* not fit to the other parameters       */
/* in this case par_sep has to be '\0'   */
static const struct one_par_def par_def = {
	.par_short = NULL,
	.par_long = NULL,
	.par_help = "this is the help for an undefined parameter",
	.par_fkt = found_default,
	.par_sep = '\0'
};

/* list of all parameters                     */
/* take care that a 'fall through'            */
/* parameter has to be defined as last entry! */
static const struct one_par_def *par_par[] = {
	&par1,
	&par2,
	&par_def,
};

/* this is the only part of the command   */
/* which must not be static.              */
/* This structure is used in all_commands */
const struct one_command cmd_parameter = {
	/* name of the command */
	.name = "parameter",

	/* short help for the command */
	.help_short = "short help",

	/* detailed help for the command */
	.help = "long help for this command",

	/* function to call */
	.fkt = do_parameter,

	/* amount of defined parameters */
	/* ALWAYS USE THIS LINE!        */
	.parameter_amount = sizeof(par_par) / sizeof(par_par[0]),

	/* parameter definition */
	.parameter = par_par,

	/* command expects at least 0 parameters */
	.argc_min = 1,

	/* command expects not more than 4 parameters */
	.argc_max = 5,
};

/* the command itself */
int do_parameter(int argc, const char *argv[])
{
	printf("This is the result of command %s\r\n", cmd_parameter.name);

	return(0);
}

/* this function is called if -p1 is found */
int found_p1(const struct one_command *called, const char *parameter, int validx)
{
	printf("%s of %s called\r\n", __func__, called->name);
	return(0);
}

/* this function is called on detection of -p2 */
int found_p2(const struct one_command *called, const char *parameter, int validx)
{
	printf("%s of %s called, parameter received: >%s<\r\n", __func__, called->name, &parameter[validx]);
	return(0);
}

/* this function is called on any not matching parameter */
int found_default(const struct one_command *called, const char *parameter, int validx)
{
	printf("%s of %s called, parameter received: >%s<\r\n", __func__, called->name, &parameter[validx]);
	return(0);
}
