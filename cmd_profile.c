#include "cmd_profile.h"
#include "cmd_std_handlers.h"
#include <stdio.h>

uint32_t cmd_var_setpoint = 200;

const cmd_command_t cmd_array[] =
{
	{"about", cmd_about_handler, "ssss", 4,
	NULL,
	"Display version information", "<strA>\0strB\0<strC>\0strD"},

	{"setpoint", cmd_var_handler, "u", 1,
	&(cmd_var_handler_data_t){.var=&cmd_var_setpoint, .min={'u', {.u=100}}, .max={'u',{.u=200}}},
	"Display/change setpoint value", "setpoint"},

	{"test", cmd_test_handler, "i", 0,
	NULL,
	"Test the system", "p1"},

	{"printargs", cmd_printargs_handler, "ssssssss", 8,
	NULL,
	"Print all arguments passed", NULL},

	{"help", cmd_help_handler, "s", 1,
	NULL,
	"Display this message", "cmd"}
};

const uint_fast8_t cmd_array_length = (sizeof(cmd_array) / sizeof(cmd_command_t));

void cmd_prompt()
{
	printf("emci[%u]>", cmd_var_setpoint);
}

cmd_status_t cmd_about_handler(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc)
{
	printf("Test Device v1.0.0\r\n");
	return CMD_STATUS_OK;
}

cmd_status_t cmd_setpoint_handler(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc)
{
	if (argc == 2)
	{
		// change setpoint
		if (argv[1].u > 65535UL)
		{
			//*extra = 1;
			return CMD_STATUS_ARG_TOO_HIGH;
		}
		else if (argv[1].u < 400UL)
		{
			//*extra = 1;
			return CMD_STATUS_ARG_TOO_LOW;
		}
		else
		{
			cmd_var_setpoint = argv[1].u;
			return CMD_STATUS_OK;
		}
	}
	else
	{
		// display current setpoint
		printf("%u\n", cmd_var_setpoint);
		return CMD_STATUS_OK;
	}
}

cmd_status_t cmd_test_handler(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc)
{
	if (argv[1].i > 1000)
	{
		//*extra = 1;
		return CMD_STATUS_ARG_TOO_HIGH;
	}
	else if (argv[1].i < 500)
	{
		//*extra = 1;
		return CMD_STATUS_ARG_TOO_LOW;
	}

	printf("test_handler: %d\r\n", argv[1].i);
	return CMD_STATUS_OK;
}

void cmd_response_handler(uint8_t argc, char *raw_argv[], cmd_status_t status, uint32_t param)
{
	printf("~%s %d (%s", raw_argv[0], status, cmd_status_message(status));
	switch(status)
	{
		case CMD_STATUS_ARG_TOO_MANY:
		case CMD_STATUS_ARG_TOO_FEW:
			// param is closest valid number of arguments
			printf(": required %u but passed %u)\r\n", param, argc-1);
			break;
		case CMD_STATUS_ARG_FORMAT:
		case CMD_STATUS_ARG_INVALID:
		case CMD_STATUS_ARG_TOO_HIGH:
		case CMD_STATUS_ARG_TOO_LOW:
			// param is number of invalid argument
			printf(": %s)\r\n", raw_argv[param]);
			break;
		default:
			printf(")\r\n");
			break;
	}
}
