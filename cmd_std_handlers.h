#ifndef CMD_STD_HANDLERS_H
#define CMD_STD_HANDLERS_H

#include "cmd_parser.h"

typedef struct
{
	void *var;
	cmd_arg_t min;
	cmd_arg_t max;
} cmd_var_handler_data_t;

cmd_status_t cmd_printargs_handler	(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc);
cmd_status_t cmd_help_handler(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc);
cmd_status_t cmd_var_handler(uint8_t argc, cmd_arg_t *argv, cmd_command_t *pc);

#endif
