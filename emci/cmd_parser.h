#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include "cmd_arg.h"

typedef struct cmd_response_tag
{
	cmd_status_t status;
	uint8_t param;
	char *msg;
} cmd_response_t;

struct cmd_env_tag;

typedef struct cmd_command_tag
{
	const char *name;
	cmd_status_t (*handler)(uint8_t argc, cmd_arg_t *argv, struct cmd_env_tag *env);
	const char *arg_types;
	uint8_t arg_optional;
	void *extra;
	const char *cmd_dscr;
	const char *arg_dscr;
} cmd_command_t;

typedef struct cmd_env_tag
{
	char cmd_buffer[CMD_MAX_LINE_LENGTH+1]; // plus 1 for '\0'
	cmd_arg_t arg_buffer[CMD_MAX_ARGS+1]; // plus 1 for command name
	cmd_command_t *cmd;
	cmd_response_t resp;
} cmd_env_t;

void cmd_main_loop(cmd_env_t *env);
void cmd_process_command(char *command, cmd_env_t *env);
const char *cmd_status_message(cmd_status_t status);
uint_fast8_t cmd_tokenize(char *buffer,
                          char del,
                          bool quotes,
                          char **tokens,			// [out] start positions of found tokens
                          uint_fast8_t token_max);	// max *tokens buffer capacity
cmd_status_t cmd_strtoul2(const char *s, uint32_t *result, uint32_t radix);
cmd_status_t cmd_strtol2(const char *s, int32_t *result, uint32_t radix);
cmd_status_t cmd_strtof2(const char *s, float *result);

#endif
