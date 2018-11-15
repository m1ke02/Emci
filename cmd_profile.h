#ifndef CMD_PROFILE_H
#define CMD_PROFILE_H

#include "cmd_parser.h"

#define CMD_ECHO_INPUT			0
#define CMD_MAX_LINE_LENGTH		32
#define CMD_MAX_COMMANDS		8
#define CMD_MAX_ARGS			8		// see "if (!adp)" line inside cmd_help_handler()
#define CMD_MAX_NAME_LENGTH		"10"	// note the quotes

// custom printf function
#define cmd_printf(...) printf(__VA_ARGS__)

void cmd_prompt();
cmd_status_t cmd_about_handler		(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc);
cmd_status_t cmd_setpoint_handler	(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc);
cmd_status_t cmd_test_handler		(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc);

void cmd_response_handler			(uint8_t argc, char *raw_argv[], cmd_status_t status, uint32_t param);

#endif
