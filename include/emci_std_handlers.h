#ifndef EMCI_STD_HANDLERS_H
#define EMCI_STD_HANDLERS_H

#include "emci_parser.h"

typedef struct emci_var_handler_data_tag
{
    void *var;
    emci_arg_type_t type;
    emci_arg_t min;
    emci_arg_t max;
    union
    {
        uint8_t prec;
        uint8_t length;
    };
    emci_status_t (*verify_handler)(emci_arg_t arg, struct emci_var_handler_data_tag *data);
} emci_var_handler_data_t;

#ifdef __cplusplus
extern "C" {
#endif

emci_status_t emci_printargs_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env);
emci_status_t emci_help_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env);
emci_status_t emci_exit_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env);
emci_status_t emci_var_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env);
emci_status_t emci_vars_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env);

#ifdef __cplusplus
}
#endif

#endif
