#ifndef CMD_PROFILE_H
#define CMD_PROFILE_H

#include "cmd_parser.h"

// custom printf function
#define cmd_printf(...) printf(__VA_ARGS__)

cmd_status_t cmd_universal_handler(uint32_t argc, cmd_arg_t *argv, uint32_t *extra);
cmd_status_t cmd_about_handler(uint32_t argc, cmd_arg_t *argv, uint32_t *extra);
cmd_status_t cmd_setdelay_handler(uint32_t argc, cmd_arg_t *argv, uint32_t *extra);
cmd_status_t cmd_getdelay_handler(uint32_t argc, cmd_arg_t *argv, uint32_t *extra);
cmd_status_t cmd_test_handler(uint32_t argc, cmd_arg_t *argv, uint32_t *extra);
void cmd_response_handler(uint32_t argc, char *argv[], cmd_status_t status, uint32_t extra);

#endif
