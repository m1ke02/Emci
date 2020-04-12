#include "emci_std_handlers.h"
#include <stdio.h>
#include <string.h>

static void cmd_help_handler0(uint_fast8_t i);

extern const cmd_command_t cmd_array[];
extern const uint_fast8_t cmd_array_length;

cmd_status_t cmd_printargs_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env)
{
	if (argc <= 1)
	{
		printf(" No arguments" CMD_ENDL);
	}
	else for (uint32_t i = 1; i < argc; i ++)
	{
		printf(" P%02d=", i);
		cmd_arg_print(&(argv[i]));
		printf(CMD_ENDL);
	}

	return CMD_STATUS_OK;
}

cmd_status_t cmd_help_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env)
{
	uint_fast8_t i;

	if (argc == 2)
	{
		// find specific command
		for (i = 0; i < cmd_array_length; i ++)
			if (strcmp(cmd_array[i].name, argv[1].s) == 0)
			{
				cmd_help_handler0(i);
				return CMD_STATUS_OK;
			}
		env->resp.param = 1;
		return CMD_STATUS_ARG_INVALID;
	}
	else
	{
		// list all commands
		printf("%"CMD_MAX_NAME_LENGTH"s%s" CMD_ENDL CMD_ENDL, "", " List of supported commands:");
		for (i = 0; i < cmd_array_length; i ++)
			cmd_help_handler0(i);
	}

	return CMD_STATUS_OK;
}

static void cmd_help_handler0(uint_fast8_t i)
{
	printf("%"CMD_MAX_NAME_LENGTH"s", cmd_array[i].name);
	const char *atp = cmd_array[i].arg_types;
	const char *adp = cmd_array[i].arg_dscr;
	int_fast8_t req = strlen(atp) - cmd_array[i].arg_optional;
	// use default names if not specified (only CMD_MAX_ARGS <= 8 supported!)
	if (!adp) adp = "p1\0p2\0p3\0p4\0p5\0p6\0p7\0p8\0";
	// list all args
	while (*atp)
	{
		printf((req > 0)? " %s:%s": " [%s:%s]", adp, cmd_arg_type_message((cmd_arg_type_t)*atp));
		adp += strlen(adp)+1;
		atp ++;
		req --;
	}
	printf(CMD_ENDL " %"CMD_MAX_NAME_LENGTH"s%s" CMD_ENDL CMD_ENDL, "", cmd_array[i].cmd_dscr);
}

cmd_status_t cmd_var_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env)
{
	cmd_var_handler_data_t *e = (cmd_var_handler_data_t *)env->cmd->extra;
	cmd_arg_type_t type = (cmd_arg_type_t)env->cmd->arg_types[0];
	bool cmp = false;

	// profile consistency check
	if (e->var == NULL || e->max.type != type || e->min.type != type ||
		strlen(env->cmd->arg_types) != 1 || type == CMD_ARG_STRING)
		return CMD_STATUS_PROFILE_ERROR;

	if (argc == 2)
	{
		// compare to max
		switch (type)
		{
			default:
			case CMD_ARG_UINT32: cmp = (argv[1].u > e->max.u); break;
			case CMD_ARG_INT32: cmp = (argv[1].i > e->max.i); break;
			case CMD_ARG_FLOAT: cmp = (argv[1].f > e->max.f); break;
		}

		if (cmp)
		{
			env->resp.param = 1;
			return CMD_STATUS_ARG_TOO_HIGH;
		}

		// compare to min
		switch (type)
		{
			default:
			case CMD_ARG_UINT32: cmp = (argv[1].u < e->min.u); break;
			case CMD_ARG_INT32: cmp = (argv[1].i < e->min.i); break;
			case CMD_ARG_FLOAT: cmp = (argv[1].f < e->min.f); break;
		}

		if (cmp)
		{
			env->resp.param = 1;
			return CMD_STATUS_ARG_TOO_LOW;
		}

		// assign new value
		switch (type)
		{
			default:
			case CMD_ARG_UINT32: *((uint32_t *)e->var) = argv[1].u; break;
			case CMD_ARG_INT32: *((int32_t *)e->var) = argv[1].i; break;
			case CMD_ARG_FLOAT: *((float *)e->var) = argv[1].f; break;
		}
		return CMD_STATUS_OK;
	}
	else /*if (argc == 1)*/
	{
		char fmt[] = "%.0f" CMD_ENDL;
		// display current value
		switch (type)
		{
			default:
			case CMD_ARG_UINT32: printf("%u" CMD_ENDL, *((uint32_t *)e->var)); break;
			case CMD_ARG_INT32: printf("%d" CMD_ENDL, *((int32_t *)e->var)); break;
			case CMD_ARG_FLOAT:
				fmt[2] = '0' + ((e->prec <= 9)? e->prec: 9);
				printf(fmt, *((float *)e->var));
				break;
		}
		return CMD_STATUS_OK;
	}
}
