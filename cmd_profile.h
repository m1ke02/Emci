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

cmd_status_t cmd_universal_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra);
cmd_status_t cmd_about_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra);
cmd_status_t cmd_setdelay_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra);
cmd_status_t cmd_getdelay_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra);
cmd_status_t cmd_test_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra);
void cmd_response_handler(uint8_t argc, char *argv[], cmd_status_t status, uint32_t *extra);

#endif
