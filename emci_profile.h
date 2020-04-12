#ifndef CMD_PROFILE_H
#define CMD_PROFILE_H

#include "cmd_parser.h"

typedef enum
{
    APP_OK = CMD_STATUS_OK,
    APP_IO_ERROR = CMD_STATUS_APP_ERROR_START,
    APP_FREQ_ERROR
} app_status_t;

void cmd_prompt(void);
cmd_status_t about_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env);
cmd_status_t test_handler(uint8_t argc, cmd_arg_t *argv, cmd_env_t *env);
const char *cmd_app_status_message(cmd_status_t status);

#endif
