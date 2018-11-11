#include "cmd_profile.h"
#include <stdio.h>

cmd_command_t cmd_array[] = {
	{"about", cmd_about_handler, "ssss", 4, "Displays version information", "<strA>\0strB\0<strC>\0strD"},
	{"setdelay", cmd_universal_handler, "ufib", 1, "Sets time delay", "u1\0f2\0i3\0b4"},
	{"getdelay", cmd_getdelay_handler, "", 0, "Displays current time delay", NULL},
	{"test", cmd_test_handler, "i", 0, "Tests the system", "p1"},
	{"strdisplay", cmd_universal_handler, "ssssssss", 8, "Displays all strings passed", NULL},
	{"help", cmd_help_handler, "s", 1, "Displays this message", "cmd"}
};

uint_fast8_t cmd_array_length = (sizeof(cmd_array) / sizeof(cmd_command_t));

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

cmd_status_t cmd_setdelay_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra)
{
	printf("setdelay_handler\r\n");
	return CMD_STATUS_OK;
}

cmd_status_t cmd_getdelay_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra)
{
	printf("getdelay_handler\r\n");
	return CMD_STATUS_OK;
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
