#include "cmd_parser.h"
#include "cmd_profile.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define CMD_COMMAND_DEL ';'
#define CMD_ARG_DEL ' '

static char cmd_buffer[CMD_MAX_LINE_LENGTH+1]; // plus 1 for '\0'
static cmd_arg_t arg_buffer[CMD_MAX_ARGS+1]; // plus 1 for command name

static cmd_status_t cmd_arg_convert(const char *s, cmd_arg_type_t type, cmd_arg_t *v);
static void cmd_help_handler0(uint_fast8_t i);

extern cmd_command_t cmd_array[];
extern uint_fast8_t cmd_array_length;

void cmd_main_loop()
{
	int c;
	char *ptr;
	char *tokens[CMD_MAX_COMMANDS];

	while (1)
	{
		// prepare to command line parsing
		ptr = cmd_buffer;

		// read command line char by char
		while (1)
		{
			if ((c = fgetc(stdin)) == EOF)
				return;
			
			// backspace key support for terminal
			if (c == '\x7F' && ptr > cmd_buffer)
			{
				ptr --;
				putc('\x7F', stdout);
				continue;
			}

			// remove several special characters
			if (c == '\r' || c == '\t')
				continue;

			#if CMD_ECHO_INPUT == 1
			putc(c, stdout);
			#endif

			if (c == '\n')
			{
				// terminate last command
				*ptr = '\0';

				uint_fast8_t ntokens = cmd_tokenize(cmd_buffer, CMD_COMMAND_DEL, true,
					tokens, sizeof(tokens) / sizeof(tokens[0]));

				// cmd handler...
				for (uint_fast8_t i = 0; i < ntokens; i ++)
				{
					printf("cmd%d=[%s]\n", i, tokens[i]);
					cmd_process_command(tokens[i]);
				}

				// we are done, go to next line
				break;
			}
			else if (ptr - cmd_buffer < CMD_MAX_LINE_LENGTH)
			{
				*ptr++ = c;
			}
		}
	}
}

void cmd_process_command(char *command)
{
	char *tokens[CMD_MAX_ARGS+1]; // plus 1 for command name

	// get tokens for command & args
	uint_fast8_t ntokens = cmd_tokenize(command, CMD_ARG_DEL, false,
		tokens, sizeof(tokens) / sizeof(tokens[0]));

	for (uint_fast8_t i = 0; i < ntokens; i ++)
	{
		printf("   arg%d=[%s]\n", i, tokens[i]);
	}

	if (ntokens > 0)
	{
		// command parsing/execution status
		cmd_status_t status = CMD_STATUS_OK;
		uint32_t extra = 0;

		// search for requested command
		uint_fast8_t cmd_num;
		for (cmd_num = 0; cmd_num < cmd_array_length; cmd_num ++)
			if (strcmp(cmd_array[cmd_num].name, tokens[0]) == 0)
				break;
		if (cmd_num >= cmd_array_length)
			status = CMD_STATUS_UNKNOWN_CMD;

		if (status == CMD_STATUS_OK)
		{
			// command found
			int_fast8_t nargs_max = strlen(cmd_array[cmd_num].arg_types);
			int_fast8_t nargs_min = nargs_max - cmd_array[cmd_num].arg_optional;

			// check if arg specification is valid
			if (nargs_max > CMD_MAX_ARGS || nargs_min < 0)
			{
				extra = (uint32_t)cmd_num;
				status = CMD_STATUS_EXEC_FAILED;
			}

			// check argument count
			if (ntokens < nargs_min+1)
			{
				extra = nargs_min; // provide nearest correct argument count
				status = CMD_STATUS_ARG_TOO_FEW;
			}
			else if (ntokens > nargs_max+1)
			{
				extra = nargs_max; // provide nearest correct argument count
				status = CMD_STATUS_ARG_TOO_MANY;
			}

			if (status == CMD_STATUS_OK)
			{
				// convert name & args from strings to cmd_arg_t variants
				cmd_arg_convert(tokens[0], CMD_ARG_STRING, &(arg_buffer[0]));
				for (uint_fast8_t t = 1; t < ntokens; t ++)
				{
					status = cmd_arg_convert(tokens[t], cmd_array[cmd_num].arg_types[t-1], &(arg_buffer[t]));
					if (status != CMD_STATUS_OK)
					{
						extra = t;
						break;
					}
				}

				if (status == CMD_STATUS_OK)
				{
					// call the command handler
					extra = cmd_num; // provide command number for extracting additional info
					status = cmd_array[cmd_num].handler(ntokens, arg_buffer, &extra);
				}
			}
		}

		cmd_response_handler(ntokens, tokens, status, &extra);
	}
}

const char *cmd_status_message(cmd_status_t status)
{
	switch (status)
	{
		case CMD_STATUS_OK: return "OK";
		case CMD_STATUS_UNKNOWN_CMD: return "Unknown command";
		case CMD_STATUS_ARG_TOO_MANY: return "Too many arguments";
		case CMD_STATUS_ARG_TOO_FEW: return "Too few arguments";
		case CMD_STATUS_ARG_FORMAT: return "Incorrect argument format";
		case CMD_STATUS_ARG_INVALID: return "Invalid argument";
		case CMD_STATUS_ARG_TOO_LOW: return "Parameter value too low";
		case CMD_STATUS_ARG_TOO_HIGH: return "Parameter value too high";
		case CMD_STATUS_EXEC_FAILED: return "Execution failed";
		default: return "?";
	}
}

const char *cmd_arg_type_message(cmd_arg_type_t type)
{
	switch (type)
	{
		case CMD_ARG_VOID: return "void";
		case CMD_ARG_UINT32: return "uint32";
		case CMD_ARG_INT32: return "int32";
		case CMD_ARG_FLOAT: return "float";
		case CMD_ARG_BOOL: return "bool";
		case CMD_ARG_STRING: return "str";
		default: return "?";
	}
}

static cmd_status_t cmd_arg_convert(const char *s, cmd_arg_type_t type, cmd_arg_t *v)
{
	cmd_status_t status;

	v->type = type;
	switch (type)
	{
		case CMD_ARG_UINT32:
			status = cmd_strtoul2(s, &(v->u), 0);
			if (status != CMD_STATUS_OK) return status;
			break;
		case CMD_ARG_INT32:
			status = cmd_strtol2(s, &(v->i), 0);
			if (status != CMD_STATUS_OK) return status;
			break;
		case CMD_ARG_FLOAT:
			status = cmd_strtof2(s, &(v->f));
			if (status != CMD_STATUS_OK) return status;
			break;
		case CMD_ARG_BOOL:
			if (strcmpi(s, "true") == 0 || strcmpi(s, "1") == 0)
				v->b = true;
			else if (strcmpi(s, "false") == 0 || strcmpi(s, "0") == 0)
				v->b = false;
			else
				return CMD_STATUS_ARG_FORMAT;
			break;
		case CMD_ARG_STRING:
			// don't copy string, just copy the pointer
			v->s = (char *)s;
			break;
		case CMD_ARG_VOID:
			v->u = 0;
			break;
	}

	return CMD_STATUS_OK;
}

uint32_t cmd_arg_print(const cmd_arg_t *v)
{
	const char *type = cmd_arg_type_message(v->type);

	switch (v->type)
	{
		case CMD_ARG_UINT32:
			return printf("%s[%u]", type, v->u);
		case CMD_ARG_INT32:
			return printf("%s[%d]", type, v->i);
		case CMD_ARG_FLOAT:
			return printf("%s[%f]", type, v->f);
		case CMD_ARG_BOOL:
			return printf(v->b? "%s[true]": "%s[false]", type);
		case CMD_ARG_STRING:
			return printf("%s[%s]", type, v->s);
		case CMD_ARG_VOID:
			return printf("%s[]", type);
		default:
			return printf("%s", type);
	}
}

uint_fast8_t cmd_tokenize(char *buffer,
                          char del,
                          bool quotes,
                          char **tokens,
                          uint_fast8_t token_max)
{
	bool q1 = false;		// 'inside single quotes' flag
	bool q2 = false;		// "inside double quotes" flag
	bool q1_prev = false;	// q1 delayed
	bool q2_prev = false;	// q2 delayed
	bool inside = false;	// 'currently inside a token' flag
	char *pi = buffer;		// buffer read pointer
	char *po = buffer;		// buffer write pointer
	uint_fast8_t token_ctr = 0;

	while (*pi != '\0')
	{
		if (*pi == '\'' && !q2)
			q1 ^= true;
		if (*pi == '\"' && !q1)
			q2 ^= true;

		// check if we are inside single or double quotes:
		// example: test"qq'qq'qq"eee'bla"bla"bla'nnn
		// iq1:     _________________^^^^^^^^^^^^^___
		// iq2:     ____^^^^^^^^^^___________________
		// prev values are required for correct skipping
		bool iq1 = q1 || q1_prev;
		bool iq2 = q2 || q2_prev;

		// control quotes are skipped here if quotes == false
		if (quotes || !((*pi == '\'' && iq1) || (*pi == '\"' && iq2)))
		{
			// add new character to the buffer
			*po = *pi;

			// check for a token start
			if (!inside && *pi != del && (po == buffer || po[-1] == del || po[-1] == '\0'))
			{
				// token start found, add to the token list
				if (token_ctr < token_max)
					tokens[token_ctr ++] = po;
				inside = true;
			}

			// when outside quotes, check for a token end and mark it with '\0'
			if (inside && !iq1 && !iq2 && *pi == del && po > buffer && po[-1] != del)
			{
				*po = '\0';
				inside = false;
			}

			po ++;
		}

		pi ++;
		q1_prev = q1;
		q2_prev = q2;
	}

	// terminate last token
	*po = '\0';
	return token_ctr;
}

cmd_status_t cmd_strtoul2(const char *s, uint32_t *result, uint32_t radix)
{
	// skip whitespace
	while (isspace(*s)) ++s;

	const char* end = s;
	errno = 0;
	*result = strtoul(s, (char **)&end, radix);

	if (*s == '-')
		return CMD_STATUS_ARG_TOO_LOW;
	else if (errno == ERANGE)
		return CMD_STATUS_ARG_TOO_HIGH;
	else if (*end != '\0')
		return CMD_STATUS_ARG_FORMAT;
	else
		return CMD_STATUS_OK;
}

cmd_status_t cmd_strtol2(const char *s, int32_t *result, uint32_t radix)
{
	const char* end = s;
	errno = 0;
	*result = strtol(s, (char **)&end, radix);

	if (errno == ERANGE)
		return (*result == INT32_MAX)? CMD_STATUS_ARG_TOO_HIGH: CMD_STATUS_ARG_TOO_LOW;
	else if (*end != '\0')
		return CMD_STATUS_ARG_FORMAT;
	else
		return CMD_STATUS_OK;
}

cmd_status_t cmd_strtof2(const char *s, float *result)
{
	const char* end = s;
	errno = 0;
	*result = strtof(s, (char **)&end);

	if (errno == ERANGE)
		return (*result == HUGE_VALF)? CMD_STATUS_ARG_TOO_HIGH: CMD_STATUS_ARG_TOO_LOW;
	else if (*end != '\0')
		return CMD_STATUS_ARG_FORMAT;
	else
		return CMD_STATUS_OK;
}

cmd_status_t cmd_help_handler(uint8_t argc, cmd_arg_t *argv, uint32_t *extra)
{
	uint_fast8_t i;

	if (argc == 2)
	{
		// find specific command
		for (i = 0; i < cmd_array_length; i ++)
			if (strcmp(cmd_array[i].name, argv[1].s) == 0)
			{
				cmd_help_handler0(i);
				return CMD_STATUS_OK;
			}
		*extra = 1;
		return CMD_STATUS_ARG_INVALID;
	}
	else
	{
		// list all commands
		printf("%"CMD_MAX_NAME_LENGTH"s%s\n\n", "", " List of supported commands:");
		for (i = 0; i < cmd_array_length; i ++)
			cmd_help_handler0(i);
	}

	return CMD_STATUS_OK;
}

static void cmd_help_handler0(uint_fast8_t i)
{
	printf("%"CMD_MAX_NAME_LENGTH"s", cmd_array[i].name);
	const char *atp = cmd_array[i].arg_types;
	const char *adp = cmd_array[i].arg_dscr;
	int_fast8_t req = strlen(atp) - cmd_array[i].arg_optional;
	// use default names if not specified (only CMD_MAX_ARGS <= 8 supported!)
	if (!adp) adp = "p1\0p2\0p3\0p4\0p5\0p6\0p7\0p8\0";
	// list all args
	while (*atp)
	{
		printf((req > 0)? " %s:%s": " [%s:%s]", adp, cmd_arg_type_message(*atp));
		adp += strlen(adp)+1;
		atp ++;
		req --;
	}
	printf("\n %"CMD_MAX_NAME_LENGTH"s%s\n\n", "", cmd_array[i].cmd_dscr);
}
