#include "emci_arg.h"
#include "emci_parser.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

const char *emci_arg_type_message(emci_arg_type_t type)
{
    switch (type)
    {
        case EMCI_ARG_VOID: return "void";
        case EMCI_ARG_UINT32: return "uint32";
        case EMCI_ARG_INT32: return "int32";
        case EMCI_ARG_FLOAT: return "float";
        case EMCI_ARG_BOOL: return "bool";
        case EMCI_ARG_STRING: return "string";
        default: return "?";
    }
}

emci_status_t emci_arg_convert(const char *s, emci_arg_type_t type, emci_arg_t *v)
{
    emci_status_t status;

    v->type = type;
    switch (type)
    {
        case EMCI_ARG_UINT32:
            status = emci_strtoul2(s, &(v->u), 0);
            if (status != EMCI_STATUS_OK) return status;
            break;
        case EMCI_ARG_INT32:
            status = emci_strtol2(s, &(v->i), 0);
            if (status != EMCI_STATUS_OK) return status;
            break;
        case EMCI_ARG_FLOAT:
            status = emci_strtof2(s, &(v->f));
            if (status != EMCI_STATUS_OK) return status;
            break;
        case EMCI_ARG_BOOL:
            if (strcmp(s, "true") == 0 || strcmp(s, "1") == 0)
                v->b = true;
            else if (strcmp(s, "false") == 0 || strcmp(s, "0") == 0)
                v->b = false;
            else
                return EMCI_STATUS_ARG_FORMAT;
            break;
        case EMCI_ARG_STRING:
            // don't copy string, just copy the pointer
            v->s = (char *)s;
            break;
        case EMCI_ARG_VOID:
            v->u = 0;
            break;
    }

    return EMCI_STATUS_OK;
}

bool emci_arg_print(emci_arg_t arg, uint8_t prec, emci_env_t *env)
{
    switch (arg.type)
    {
        case EMCI_ARG_STRING: return emci_print_value(arg.s, arg.type, prec, env);
        case EMCI_ARG_BOOL: return emci_print_value(&arg.b, arg.type, prec, env);
        case EMCI_ARG_UINT32: return emci_print_value(&arg.u, arg.type, prec, env);
        case EMCI_ARG_INT32: return emci_print_value(&arg.i, arg.type, prec, env);
        case EMCI_ARG_FLOAT: return emci_print_value(&arg.f, arg.type, prec, env);
        case EMCI_ARG_VOID: EMCI_PRINTF("<void>"); return true;
        default: return false;
    }
}

bool emci_print_value(const void *data, emci_arg_type_t type, uint8_t prec, emci_env_t *env)
{
    char fmt[] = "%.0f";

    switch (type)
    {
        case EMCI_ARG_STRING: EMCI_PRINTF("%s", (char *)data); break;
        case EMCI_ARG_BOOL: EMCI_PRINTF(*((bool *)data) ? "true" : "false"); break;
        case EMCI_ARG_UINT32: EMCI_PRINTF("%"PRIu32, *((uint32_t *)data)); break;
        case EMCI_ARG_INT32: EMCI_PRINTF("%"PRId32, *((int32_t *)data)); break;
        case EMCI_ARG_FLOAT:
            fmt[2] = '0' + ((prec <= 9)? prec: 9);
            EMCI_PRINTF(fmt, *((float *)data));
            break;
        case EMCI_ARG_VOID: EMCI_PRINTF("<void>"); break;
        default: return false;
    }
    return true;
}
