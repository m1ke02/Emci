#ifndef CMD_COMMON_H
#define CMD_COMMON_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	CMD_STATUS_OK = 0,
	CMD_STATUS_UNKNOWN_CMD = 1,
	CMD_STATUS_PROFILE_ERROR = 2,
	CMD_STATUS_ARG_TOO_MANY = 3,
	CMD_STATUS_ARG_TOO_FEW = 4,
	CMD_STATUS_ARG_FORMAT = 5,
	CMD_STATUS_ARG_INVALID = 6,
	CMD_STATUS_ARG_TOO_LOW = 7,
	CMD_STATUS_ARG_TOO_HIGH = 8,
	CMD_STATUS_APP_ERROR_START = 32,
	CMD_STATUS_APP_ERROR_END = 255
} cmd_status_t;

#define CMD_COMMAND_DEL			';'
#define CMD_ARG_DEL				' '
#define CMD_ENTER_KEY			'\n'
#define CMD_ENDL				"\r\n"
#define CMD_ECHO_INPUT			0
#define CMD_MAX_LINE_LENGTH		32
#define CMD_MAX_COMMANDS		8
#define CMD_MAX_ARGS			10		// see "if (!adp)" line inside cmd_help_handler()
#define CMD_MAX_NAME_LENGTH		"10"	// note the quotes

#endif