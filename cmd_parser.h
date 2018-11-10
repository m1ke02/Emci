#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	CMD_ARG_VOID = '\0',
	CMD_ARG_UINT32 = 'u',
	CMD_ARG_INT32 = 'i',
	CMD_ARG_FLOAT = 'f',
	CMD_ARG_BOOL = 'b',
	CMD_ARG_STRING = 's'
} cmd_arg_type_t;

typedef enum
{
	CMD_STATUS_OK = 0,
	CMD_STATUS_UNKNOWN_CMD = -1,
	CMD_STATUS_ARG_TOO_MANY = -2,
	CMD_STATUS_ARG_TOO_FEW = -3,
	CMD_STATUS_ARG_FORMAT = -4,
	CMD_STATUS_ARG_TOO_LOW = -5,
	CMD_STATUS_ARG_TOO_HIGH = -6,
	CMD_STATUS_EXEC_FAILED = -100
} cmd_status_t;

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

typedef struct
{
	const char *name;
	cmd_status_t (*handler)(uint32_t argc, cmd_arg_t *argv, uint32_t *extra);
	const char *arg_types;
	uint32_t arg_optional;
	const char *cmd_dscr;
	const char *arg_dscr;
} cmd_command_t;

void cmd_main_loop();
void cmd_process_line(char *line);
const char *cmd_status_message(cmd_status_t status);
const char *cmd_arg_type_message(cmd_arg_type_t type);
uint32_t cmd_arg_print(const cmd_arg_t *v);
cmd_status_t cmd_strtoul2(const char *s, uint32_t *result, uint32_t radix);
cmd_status_t cmd_strtol2(const char *s, int32_t *result, uint32_t radix);
cmd_status_t cmd_strtof2(const char *s, float *result);
cmd_status_t cmd_help_handler(uint32_t argc, cmd_arg_t *argv, uint32_t *extra);

#endif
