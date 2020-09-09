#include "emci_arg.h"
#include "emci_parser.h"
#include <stdio.h>
#include <string.h>

const char *emci_arg_type_message(emci_arg_type_t type)
{
	switch (type)
	{
		case EMCI_ARG_VOID: return "void";
		case EMCI_ARG_UINT32: return "uint32";
		case EMCI_ARG_INT32: return "int32";
		case EMCI_ARG_FLOAT: return "float";
		case EMCI_ARG_BOOL: return "bool";
		case EMCI_ARG_STRING: return "str";
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

uint32_t emci_arg_print(const emci_arg_t *v)
{
	const char *type = emci_arg_type_message(v->type);

	switch (v->type)
	{
		case EMCI_ARG_UINT32:
			return EMCI_PRINTF("%s[%u]", type, v->u);
		case EMCI_ARG_INT32:
			return EMCI_PRINTF("%s[%d]", type, v->i);
		case EMCI_ARG_FLOAT:
			return EMCI_PRINTF("%s[%f]", type, v->f);
		case EMCI_ARG_BOOL:
			return EMCI_PRINTF(v->b? "%s[true]": "%s[false]", type);
		case EMCI_ARG_STRING:
			return EMCI_PRINTF("%s[%s]", type, v->s);
		case EMCI_ARG_VOID:
			return EMCI_PRINTF("%s[]", type);
		default:
			return EMCI_PRINTF("%s", type);
	}
}
