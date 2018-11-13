#include "cmd_profile.h"
#include <stdio.h>

uint16_t cmd_var_setpoint = 200;

cmd_command_t cmd_array[] = {
	{"about", cmd_about_handler, "ssss", 4, NULL, "Display version information", "<strA>\0strB\0<strC>\0strD"},
	{"setpoint", cmd_setpoint_handler, "u", 1, NULL, "Display/change setpoint value", "setpoint"},
	{"test", cmd_test_handler, "i", 0, NULL, "Test the system", "p1"},
	{"strdisplay", cmd_universal_handler, "ssssssss", 8, NULL, "Display all strings passed", NULL},
	{"help", cmd_help_handler, "s", 1, NULL, "Display this message", "cmd"}
};

uint_fast8_t cmd_array_length = (sizeof(cmd_array) / sizeof(cmd_command_t));

void cmd_prompt()
{
	printf("emci[%u]>", cmd_var_setpoint);
}

cmd_status_t cmd_universal_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra)
{
	printf("Universal handler: %s\n", argv[0].s);

	if (argc <= 1)
	{
		printf(" No arguments\n");
	}
	else for (uint32_t i = 1; i < argc; i ++)
	{
		printf(" P%02d=", i);
		cmd_arg_print(&(argv[i]));
		printf("\n");
	}

	return CMD_STATUS_OK;
}

cmd_status_t cmd_about_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra)
{
	printf("Test Device v1.0.0\r\n");
	return CMD_STATUS_OK;
}

cmd_status_t cmd_setpoint_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra)
{
	if (argc == 2)
	{
		// change setpoint
		if (argv[1].u > 65535UL)
		{
			*extra = 1;
			return CMD_STATUS_ARG_TOO_HIGH;
		}
		else if (argv[1].u < 400UL)
		{
			*extra = 1;
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

cmd_status_t cmd_test_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra)
{
	if (argv[1].i > 1000)
	{
		*extra = 1;
		return CMD_STATUS_ARG_TOO_HIGH;
	}
	else if (argv[1].i < 500)
	{
		*extra = 1;
		return CMD_STATUS_ARG_TOO_LOW;
	}

	printf("test_handler: %d\r\n", argv[1].i);
	return CMD_STATUS_OK;
}

void cmd_response_handler(uint8_t argc, char *argv[], cmd_status_t status, uint32_t *extra)
{
	printf("~%s %d (%s", argv[0], status, cmd_status_message(status));
	switch(status)
	{
		case CMD_STATUS_ARG_TOO_MANY:
		case CMD_STATUS_ARG_TOO_FEW:
			printf(": required %u but passed %u)\r\n", *extra, argc-1);
			break;
		case CMD_STATUS_ARG_FORMAT:
			printf(": %s)\r\n", argv[*extra]);
			break;
		case CMD_STATUS_ARG_INVALID:
		case CMD_STATUS_ARG_TOO_HIGH:
		case CMD_STATUS_ARG_TOO_LOW:
			printf(": %s)\r\n", argv[*extra]);
			break;
		default:
			printf(")\r\n");
			break;
	}
}
