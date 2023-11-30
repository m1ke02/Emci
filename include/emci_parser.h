#ifndef EMCI_PARSER_H
#define EMCI_PARSER_H

#include "emci_arg.h"

typedef struct emci_response_tag
{
    emci_status_t status;
    uint8_t param;
    char *msg;
    bool exit;
} emci_response_t;

struct emci_env_tag;

typedef struct emci_command_tag
{
    const char *name;
    emci_status_t (*handler)(uint8_t argc, emci_arg_t *argv, struct emci_env_tag *env);
    const char *arg_types;
    uint8_t arg_optional;
    void *extra;
    const char *cmd_dscr;
    const char *arg_dscr;
} emci_command_t;

typedef struct emci_env_tag
{
    char cmd_buffer[EMCI_MAX_LINE_LENGTH+1]; // plus 1 for '\0'
    emci_arg_t arg_buffer[EMCI_MAX_ARGS+1]; // plus 1 for command name
    emci_command_t *cmd;
    emci_response_t resp;
    void *extra;
} emci_env_t;

#ifdef __cplusplus
extern "C" {
#endif

void emci_main_loop(emci_env_t *env);
void emci_process_command(char *command, emci_env_t *env);
const char *emci_status_message(emci_status_t status);
uint_fast8_t emci_tokenize(char *buffer,
                          char del,
                          bool quotes,
                          char **tokens,			// [out] start positions of found tokens
                          uint_fast8_t token_max);	// max *tokens buffer capacity
emci_status_t emci_strtoul2(const char *s, uint32_t *result, uint32_t radix);
emci_status_t emci_strtol2(const char *s, int32_t *result, uint32_t radix);
emci_status_t emci_strtof2(const char *s, float *result);

#ifdef __cplusplus
}
#endif

#endif
