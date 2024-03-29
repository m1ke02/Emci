#include "emci_std_handlers.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

static void emci_help_handler0(uint_fast8_t i, emci_env_t *env);

extern const emci_command_t cmd_array[];
extern const uint_fast8_t cmd_array_length;

emci_status_t emci_printargs_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env)
{
    if (argc <= 1)
    {
        EMCI_PRINTF(" No arguments" EMCI_ENDL);
    }
    else for (uint32_t i = 1; i < argc; i ++)
    {
        EMCI_PRINTF(" P%02"PRIu32"=", i);
        emci_arg_print(argv[i], 4, env);
        EMCI_PRINTF(EMCI_ENDL);
    }

    return EMCI_STATUS_OK;
}

emci_status_t emci_help_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env)
{
    uint_fast8_t i;

    if (argc == 2)
    {
        // find specific command
        for (i = 0; i < cmd_array_length; i ++)
            if (strcmp(cmd_array[i].name, argv[1].s) == 0)
            {
                emci_help_handler0(i, env);
                return EMCI_STATUS_OK;
            }
        env->resp.param = 1;
        return EMCI_STATUS_ARG_INVALID;
    }
    else
    {
        // list all commands
        EMCI_PRINTF("List of supported commands:" EMCI_ENDL);
        for (i = 0; i < cmd_array_length; i ++)
            emci_help_handler0(i, env);
    }

    return EMCI_STATUS_OK;
}

static void emci_help_handler0(uint_fast8_t i, emci_env_t *env)
{
    EMCI_PRINTF("%"EMCI_XSTR(EMCI_MAX_NAME_LENGTH)"s", cmd_array[i].name);

    const char *atp = cmd_array[i].arg_types;
    const char *adp = cmd_array[i].arg_dscr;
    int_fast8_t req = strlen(atp) - cmd_array[i].arg_optional;
    // use default names if not specified (only EMCI_MAX_ARGS <= 8 supported!)
    if (!adp) adp = "p1\0p2\0p3\0p4\0p5\0p6\0p7\0p8\0";
    // list all args
    while (*atp)
    {
        EMCI_PRINTF((req > 0)? " %s:%s": " [%s:%s]", adp, emci_arg_type_message((emci_arg_type_t)*atp));
        adp += strlen(adp)+1;
        atp ++;
        req --;
    }
    EMCI_PRINTF(EMCI_ENDL "%"EMCI_XSTR(EMCI_MAX_NAME_LENGTH)"s %s" EMCI_ENDL EMCI_ENDL, "", cmd_array[i].cmd_dscr);
}

emci_status_t emci_exit_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env)
{
    env->resp.exit = true;
    return EMCI_STATUS_OK;
}

emci_status_t emci_var_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env)
{
    emci_var_handler_data_t *e = (emci_var_handler_data_t *)env->cmd->extra;
    bool cmp = false;

    // general profile consistency check
    if (e->var == NULL)
        return EMCI_STATUS_PROFILE_ERROR;
    if (e->type == EMCI_ARG_VOID)
        return EMCI_STATUS_PROFILE_ERROR;
    size_t spec_argc = strlen(env->cmd->arg_types);
    if (spec_argc == 1)
    {
        // argument list contains 1 item for writeable variable (argc == 1 or 2)
        if (e->type != (emci_arg_type_t)env->cmd->arg_types[0])
            return EMCI_STATUS_PROFILE_ERROR;
        if (((e->max.type != EMCI_ARG_VOID) && (e->max.type != e->type)) ||
            ((e->min.type != EMCI_ARG_VOID) && e->min.type != e->type))
            return EMCI_STATUS_PROFILE_ERROR;
    }
    else if (spec_argc == 0)
    {
        // argument list contains no items for readonly variable (argc == 1)
    }
    else
    {
        return EMCI_STATUS_PROFILE_ERROR;
    }

    if (argc == 2)
    {
        if (e->max.type != EMCI_ARG_VOID)
        {
            switch (e->type)   // compare to max
            {
                case EMCI_ARG_STRING:
                case EMCI_ARG_BOOL: cmp = false; break;
                case EMCI_ARG_UINT32: cmp = (argv[1].u > e->max.u); break;
                case EMCI_ARG_INT32: cmp = (argv[1].i > e->max.i); break;
                case EMCI_ARG_FLOAT: cmp = (argv[1].f > e->max.f); break;
                default: env->resp.param = 1; return EMCI_STATUS_NOT_SUPPORTED;
            }
            if (cmp)
            {
                env->resp.param = 1;
                return EMCI_STATUS_ARG_TOO_HIGH;
            }
        }

        if (e->min.type != EMCI_ARG_VOID)
        {
            switch (e->type)   // compare to min
            {
                case EMCI_ARG_STRING:
                case EMCI_ARG_BOOL: cmp = false; break;
                case EMCI_ARG_UINT32: cmp = (argv[1].u < e->min.u); break;
                case EMCI_ARG_INT32: cmp = (argv[1].i < e->min.i); break;
                case EMCI_ARG_FLOAT: cmp = (argv[1].f < e->min.f); break;
                default: break;
            }
            if (cmp)
            {
                env->resp.param = 1;
                return EMCI_STATUS_ARG_TOO_LOW;
            }
        }

        if (e->verify_handler != NULL)
        {
            emci_status_t verify_status = e->verify_handler(argv[1], e);
            if (verify_status != EMCI_STATUS_OK)
            {
                env->resp.param = 1;
                return verify_status;
            }
        }

        switch (e->type)   // assign new value
        {
            case EMCI_ARG_STRING:
                strncpy((char *)e->var, argv[1].s, e->length);
                ((char *)e->var)[e->length - 1] = '\0';
                break;
            case EMCI_ARG_BOOL: *((bool *)e->var) = argv[1].b; break;
            case EMCI_ARG_UINT32: *((uint32_t *)e->var) = argv[1].u; break;
            case EMCI_ARG_INT32: *((int32_t *)e->var) = argv[1].i; break;
            case EMCI_ARG_FLOAT: *((float *)e->var) = argv[1].f; break;
            default: break;
        }
        return EMCI_STATUS_OK;
    }
    else /*if (argc == 1)*/
    {
        if (emci_print_value(e->var, e->type, e->prec, env)) // display current value
        {
            EMCI_PRINTF(EMCI_ENDL);
            return EMCI_STATUS_OK;
        }
        else
        {
            return EMCI_STATUS_NOT_SUPPORTED;
        }
    }
}

emci_status_t emci_vars_handler(uint8_t argc, emci_arg_t *argv, emci_env_t *env)
{
    uint_fast8_t i;

    EMCI_PRINTF("List of variables:" EMCI_ENDL);

    for (i = 0; i < cmd_array_length; i ++)
    {
        if (cmd_array[i].handler == emci_var_handler)
        {
            emci_var_handler_data_t *e = (emci_var_handler_data_t *)cmd_array[i].extra;
            EMCI_PRINTF("%-6s %-"EMCI_XSTR(EMCI_MAX_NAME_LENGTH)"s ", emci_arg_type_message(e->type), cmd_array[i].name);
            if (!emci_print_value(e->var, e->type, e->prec, env))
                return EMCI_STATUS_NOT_SUPPORTED;
            EMCI_PRINTF(EMCI_ENDL);
        }
    }
    return EMCI_STATUS_OK;
}
