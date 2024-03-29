#include "emci_profile.h"
#include "emci_std_handlers.h"
#include <stdio.h>

uint32_t cmd_var_setpoint = 200;
float cmd_var_fff = 123.4;
float cmd_var_delay = 888.8;
int32_t cmd_var_llim = 0;
int32_t cmd_var_ulim = 0;
int32_t cmd_var_vh = 0;
char cmd_var_str[8] = "InitVal";

emci_status_t cmd_vh_verify_handler(emci_arg_t arg, struct emci_var_handler_data_tag *data);

const emci_command_t cmd_array[] =
{
    {"about", about_handler, "", 0,
    NULL,
    "Display version information", "<strA>\0strB\0<strC>\0strD"},

    {"setpoint", emci_var_handler, "u", 1,
    &(emci_var_handler_data_t){.var=&cmd_var_setpoint, .type=EMCI_ARG_UINT32, .min={EMCI_ARG('u'), {.u=100}}, .max={EMCI_ARG('u'),{.u=200}}},
    "Display/change setpoint value", "value"},

    {"delay", emci_var_handler, "f", 1,
    &(emci_var_handler_data_t){.var=&cmd_var_delay, .type=EMCI_ARG_FLOAT, .min={EMCI_ARG('f'), {.f=0.0}}, .max={EMCI_ARG('f'), {.f=60.0}}, .prec=1},
    "Display/change delay", "delay[ms]"},

    {"fff", emci_var_handler, "f", 1,
    &(emci_var_handler_data_t){.var=&cmd_var_fff, .type=EMCI_ARG_FLOAT, .min={EMCI_ARG('f'), {.f=.1}}, .max={EMCI_ARG('f'), {.f=.9}}, .prec=4},
    "Display/change fff value", "value"},

    {"llim", emci_var_handler, "i", 1,
    &(emci_var_handler_data_t){.var=&cmd_var_llim, .type=EMCI_ARG_INT32, .min={EMCI_ARG('i'), {.i=-100}}},
    "Display/change llim value (only lower limit)", "value"},

    {"ulim", emci_var_handler, "i", 1,
    &(emci_var_handler_data_t){.var=&cmd_var_ulim, .type=EMCI_ARG_INT32, .max={EMCI_ARG('i'), {.i=100}}},
    "Display/change ulim value (only upper limit)", "value"},

    {"vh", emci_var_handler, "i", 1,
    &(emci_var_handler_data_t){.var=&cmd_var_vh, .type=EMCI_ARG_INT32, .verify_handler=cmd_vh_verify_handler},
    "Display/change vh value (custom verification)", "value"},

    {"str", emci_var_handler, "s", 1,
    &(emci_var_handler_data_t){.var=&cmd_var_str, .type=EMCI_ARG_STRING, .length=sizeof(cmd_var_str)},
    "Display/change str value", "value"},

    {"vars", emci_vars_handler, "", 0,
    NULL,
    "Print list of all variables", NULL},

    {"test", test_handler, "i", 0,
    NULL,
    "Test the system", "p1"},

    {"printargs", emci_printargs_handler, "ssssssss", 8,
    NULL,
    "Print all arguments passed", NULL},

    {"help", emci_help_handler, "s", 1,
    NULL,
    "Display this message", "cmd"},

    {"exit", emci_exit_handler, "", 0,
    NULL,
    "Exit command session", NULL}
};

const uint_fast8_t cmd_array_length = (sizeof(cmd_array) / sizeof(emci_command_t));

void emci_prompt(emci_env_t *env)
{
    EMCI_PRINTF("emci[%u]>", cmd_var_setpoint);
}

emci_status_t about_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env)
{
    EMCI_PRINTF("Test Device v1.0.0" EMCI_ENDL);
    env->resp.msg = "Frequency cannot be measured";
    return (emci_status_t)APP_FREQ_ERROR;
}

emci_status_t test_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env)
{
    if (argv[1].i > 1000)
    {
        env->resp.param = 1;
        return EMCI_STATUS_ARG_TOO_HIGH;
    }
    else if (argv[1].i < 500)
    {
        env->resp.param = 1;
        return EMCI_STATUS_ARG_TOO_LOW;
    }

    EMCI_PRINTF("test_handler: %d" EMCI_ENDL, argv[1].i);
    return EMCI_STATUS_OK;
}

emci_status_t cmd_vh_verify_handler(emci_arg_t arg, emci_var_handler_data_t *data)
{
    // accept only even numbers
    if (arg.i % 2 != 0)
        return EMCI_STATUS_ARG_INVALID;
    else
        return EMCI_STATUS_OK;
}

const char *emci_app_status_message(emci_status_t status)
{
    switch ((app_status_t)status)
    {
        case APP_OK: return "OK"; break;
        case APP_IO_ERROR: return "IO Error"; break;
        case APP_FREQ_ERROR: return "Frequency Error"; break;
        default: return "?"; break;
    }
}
