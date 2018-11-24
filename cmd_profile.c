#include "cmd_profile.h"
#include "emci/cmd_std_handlers.h"
#include <stdio.h>

uint32_t cmd_var_setpoint = 200;
float cmd_var_fff = 123.4;

const cmd_command_t cmd_array[] =
{
	{"about", about_handler, "", 0,
	NULL,
	"Display version information", "<strA>\0strB\0<strC>\0strD"},

	{"setpoint", cmd_var_handler, "u", 1,
	&(cmd_var_handler_data_t){.var=&cmd_var_setpoint, .min={'u', {.u=100}}, .max={'u',{.u=200}}},
	"Display/change setpoint value", "value"},

	{"fff", cmd_var_handler, "f", 1,
	&(cmd_var_handler_data_t){.var=&cmd_var_fff, .min={'f', {.f=.1}}, .max={'f',{.f=.9}}, .prec=4},
	"Display/change fff value", "value"},

	{"test", test_handler, "i", 0,
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

cmd_status_t about_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env)
{
	printf("Test Device v1.0.0\r\n");
	env->resp.msg = "Frequency cannot be measured";
	return APP_FREQ_ERROR;
}

cmd_status_t test_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env)
{
	if (argv[1].i > 1000)
	{
		env->resp.param = 1;
		return CMD_STATUS_ARG_TOO_HIGH;
	}
	else if (argv[1].i < 500)
	{
		env->resp.param = 1;
		return CMD_STATUS_ARG_TOO_LOW;
	}

	printf("test_handler: %d\r\n", argv[1].i);
	return CMD_STATUS_OK;
}

const char *cmd_app_status_message(cmd_status_t status)
{
	switch ((app_status_t)status)
	{
		case APP_OK: return "OK"; break;
		case APP_IO_ERROR: return "IO Error"; break;
		case APP_FREQ_ERROR: return "Frequency Error"; break;
		default: return "?"; break;
	}
}
