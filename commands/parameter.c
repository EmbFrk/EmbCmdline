#include <stdio.h>

#include <command_definition.h>

static int do_parameter(int argc, const char *argv[]);

static int found_p1(const struct one_command *called, const char *parameter, int validx);
static int found_p2(const struct one_command *called, const char *parameter, int validx);
static int found_default(const struct one_command *called, const char *parameter, int validx);

static const struct one_par_def par1 = {
	.par_short = "-p1",
	.par_long = "--parameter1",
	.par_help = "this is the help for parameter1",
	.par_fkt = found_p1,
	.par_sep = '\0'
};

static const struct one_par_def par2 = {
	.par_short = "-p2",
	.par_long = "--parameter2",
	.par_help = "this is the help for parameter2",
	.par_fkt = found_p2,
	.par_sep = '='
};

static const struct one_par_def par_def = {
	.par_short = NULL,
	.par_long = NULL,
	.par_help = "this is the help for a undefined parameter",
	.par_fkt = found_default,
	.par_sep = '\0'
};

static const struct one_par_def *par_par[] = {
	&par1,
	&par2,
	&par_def,
};

const struct one_command cmd_parameter = {
	.name = "parameter",
	.help_short = "short help",
	.help = "long help for this command",
	.fkt = do_parameter,
	.parameter_amount = sizeof(par_par) / sizeof(par_par[0]),
	.parameter = par_par,
	.argc_min = 1,
	.argc_max = 5,
};

int do_parameter(int argc, const char *argv[])
{
	printf("This is the result of command %s\r\n", cmd_parameter.name);

	return(0);
}

int found_p1(const struct one_command *called, const char *parameter, int validx)
{
	printf("%s of %s called\r\n", __func__, called->name);
	return(0);
}

int found_p2(const struct one_command *called, const char *parameter, int validx)
{
	printf("%s of %s called, parameter received: >%s<\r\n", __func__, called->name, &parameter[validx]);
	return(0);
}

int found_default(const struct one_command *called, const char *parameter, int validx)
{
	printf("%s of %s called, parameter received: >%s<\r\n", __func__, called->name, &parameter[validx]);
	return(0);
}
