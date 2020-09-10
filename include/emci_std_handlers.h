#ifndef EMCI_STD_HANDLERS_H
#define EMCI_STD_HANDLERS_H

#include "emci_parser.h"

typedef struct
{
    void *var;
    emci_arg_t min;
    emci_arg_t max;
    uint8_t prec;
} emci_var_handler_data_t;

emci_status_t emci_printargs_handler	(uint8_t argc, emci_arg_t *argv, emci_env_t *env);
emci_status_t emci_help_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env);
emci_status_t emci_var_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env);

#endif
