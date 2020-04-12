#ifndef EMCI_STD_HANDLERS_H
#define EMCI_STD_HANDLERS_H

#include "emci_parser.h"

typedef struct
{
	void *var;
	cmd_arg_t min;
	cmd_arg_t max;
	uint8_t prec;
} cmd_var_handler_data_t;

cmd_status_t cmd_printargs_handler	(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env);
cmd_status_t cmd_help_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env);
cmd_status_t cmd_var_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env);

#endif
