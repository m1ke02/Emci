#ifndef EMCI_ARG_H
#define EMCI_ARG_H

#include "emci_config.h"
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

typedef enum
{
	CMD_ARG_VOID = '\0',
	CMD_ARG_UINT32 = 'u',
	CMD_ARG_INT32 = 'i',
	CMD_ARG_FLOAT = 'f',
	CMD_ARG_BOOL = 'b',
	CMD_ARG_STRING = 's'
} cmd_arg_type_t;

typedef struct
{
	cmd_arg_type_t type;
	union
	{
		uint32_t u;
		int32_t i;
		float f;
		bool b;
		char *s;
	};
} cmd_arg_t;

#define CMD_ARG(ch) ((cmd_arg_type_t)(ch)) // removes "enumerated type mixed with another type" warning

const char *cmd_arg_type_message(cmd_arg_type_t type);
cmd_status_t cmd_arg_convert(const char *s, cmd_arg_type_t type, cmd_arg_t *v);
uint32_t cmd_arg_print(const cmd_arg_t *v);

#endif
