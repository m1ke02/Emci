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

#endif
