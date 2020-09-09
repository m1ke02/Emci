#ifndef EMCI_ARG_H
#define EMCI_ARG_H

#include "emci_config.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	EMCI_STATUS_OK = 0,
	EMCI_STATUS_UNKNOWN_CMD = 1,
	EMCI_STATUS_PROFILE_ERROR = 2,
	EMCI_STATUS_ARG_TOO_MANY = 3,
	EMCI_STATUS_ARG_TOO_FEW = 4,
	EMCI_STATUS_ARG_FORMAT = 5,
	EMCI_STATUS_ARG_INVALID = 6,
	EMCI_STATUS_ARG_TOO_LOW = 7,
	EMCI_STATUS_ARG_TOO_HIGH = 8,
	EMCI_STATUS_APP_ERROR_START = 32,
	EMCI_STATUS_APP_ERROR_END = 255
} emci_status_t;

typedef enum
{
	EMCI_ARG_VOID = '\0',
	EMCI_ARG_UINT32 = 'u',
	EMCI_ARG_INT32 = 'i',
	EMCI_ARG_FLOAT = 'f',
	EMCI_ARG_BOOL = 'b',
	EMCI_ARG_STRING = 's'
} emci_arg_type_t;

typedef struct
{
	emci_arg_type_t type;
	union
	{
		uint32_t u;
		int32_t i;
		float f;
		bool b;
		char *s;
	};
} emci_arg_t;

#define EMCI_ARG(ch) ((emci_arg_type_t)(ch)) // removes "enumerated type mixed with another type" warning

const char *emci_arg_type_message(emci_arg_type_t type);
emci_status_t emci_arg_convert(const char *s, emci_arg_type_t type, emci_arg_t *v);
uint32_t emci_arg_print(const emci_arg_t *v);

#endif
