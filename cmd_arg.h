#ifndef CMD_ARG_H
#define CMD_ARG_H

#include "cmd_common.h"

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

const char *cmd_arg_type_message(cmd_arg_type_t type);
cmd_status_t cmd_arg_convert(const char *s, cmd_arg_type_t type, cmd_arg_t *v);
uint32_t cmd_arg_print(const cmd_arg_t *v);

#endif
