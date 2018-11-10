#include "cmd_parser.h"
#include "cmd_profile.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define CMD_COMMAND_DELIMITERS ";"
#define CMD_PARAM_DELIMITERS " "

static char cmd_buffer[CMD_MAX_COMMAND_LENGTH+1]; // plus 1 for '\0'
static cmd_arg_t arg_buffer[CMD_MAX_ARGS+1]; // plus 1 for command name

static cmd_status_t cmd_arg_convert(const char *s, cmd_arg_type_t type, cmd_arg_t *v);
static void cmd_help_handler0(uint32_t i);

extern cmd_command_t cmd_array[];
extern uint32_t cmd_array_length;

void cmd_main_loop()
{
	int c;
	bool q1 = false;
	bool q1_prev = false;
	bool q2 = false;
	bool q2_prev = false;
	char *buf = cmd_buffer;
	uint32_t i = 0;

	while (1)
	{
		if ((c = fgetc(stdin)) == EOF)
			return;
		c = (char)c;

		#if CMD_ECHO_INPUT == 1
		putc(c, stdin);
		#endif

		if (c == '\'' && !q2)
			q1 ^= true;
		if (c == '\"' && !q1)
			q2 ^= true;

		// remove control quotes from resulting command string
		if ((c == '\'' && (q1 || q1_prev)) ||
			(c == '\"' && (q2 || q2_prev)))
			continue;

		// replace spaces inside quotes with space markers
		if (c == ' ' && (q1 || q2))
			c = '_';

		// remove several special characters
		if (c == '\r' || c == '\t')
			continue;

		if (c == '\n')
		{
			// process command completion
			*buf = '\0';

			// cmd handler...
			cmd_process_line(cmd_buffer);
			//printf("{%s}\n", cmd_buffer);

			// prepare to next command processing
			buf = cmd_buffer; i = 0;
			q1 = false;
			q2 = false;
		}
		else if (i < CMD_MAX_COMMAND_LENGTH)
		{
			// add character to the command buffer
			*buf++ = (char)c;
			i ++;
		}

		q1_prev = q1;
		q2_prev = q2;
	}
}

void cmd_process_line(char *line)
{
	char *cmd;
	char *tokens[CMD_MAX_ARGS+1]; // plus 1 for command name

	/* walk through commands */
	char *cmd_iter = line;
	while ((cmd = strtok_r(cmd_iter, CMD_COMMAND_DELIMITERS, &cmd_iter)))
	{
		/* walk through command tokens */
		char *token_iter = cmd;
		uint32_t num_tokens = 0;
		while ((num_tokens < CMD_MAX_ARGS+1) &&
			(tokens[num_tokens] = strtok_r(token_iter, CMD_PARAM_DELIMITERS, &token_iter)))
			num_tokens ++;

		if (num_tokens > 0)
		{
			// command parsing/execution status
			cmd_status_t status = CMD_STATUS_OK;
			uint32_t extra = 0;

			// search for requested command
			int32_t cmd_num;
			for (cmd_num = 0; cmd_num < cmd_array_length; cmd_num ++)
				if (strcmp(cmd_array[cmd_num].name, tokens[0]) == 0)
					break;
			if (cmd_num >= cmd_array_length)
				status = CMD_STATUS_UNKNOWN_CMD;

			if (status == CMD_STATUS_OK)
			{
				// command found
				int32_t nargs_max = strlen(cmd_array[cmd_num].arg_types);
				int32_t nargs_min = nargs_max - cmd_array[cmd_num].arg_optional;

				// check if arg specification is valid
				if (nargs_max > CMD_MAX_ARGS || nargs_min < 0)
				{
					extra = (uint32_t)cmd_num;
					status = CMD_STATUS_EXEC_FAILED;
				}

				// check argument count
				if (num_tokens < nargs_min+1)
				{
					extra = nargs_min; // provide nearest correct argument count
					status = CMD_STATUS_ARG_TOO_FEW;
				}
				else if (num_tokens > nargs_max+1)
				{
					extra = nargs_max; // provide nearest correct argument count
					status = CMD_STATUS_ARG_TOO_MANY;
				}

				if (status == CMD_STATUS_OK)
				{
					// convert name & args from strings to cmd_arg_t variants
					cmd_arg_convert(tokens[0], CMD_ARG_STRING, &(arg_buffer[0]));
					for (uint32_t t = 1; t < num_tokens; t ++)
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
						status = cmd_array[cmd_num].handler(num_tokens, arg_buffer, &extra);
					}
				}
			}

			cmd_response_handler(num_tokens, tokens, status, extra);
		}
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

cmd_status_t cmd_help_handler(uint32_t argc, cmd_arg_t *argv, uint32_t *extra)
{
	uint32_t i;

	if (argc == 2)
	{
		// find specific command
		for (i = 0; i < cmd_array_length; i ++)
			if (strcmp(cmd_array[i].name, argv[1].s) == 0)
			{
				cmd_help_handler0(i);
				return CMD_STATUS_OK;
			}
		return CMD_STATUS_UNKNOWN_CMD;
	}
	else
	{
		// list all commands
		printf("%"CMD_MAX_NAME_LENGTH"s%s\n\n", "", "List of supported commands:");
		for (i = 0; i < cmd_array_length; i ++)
			cmd_help_handler0(i);
	}

	return CMD_STATUS_OK;
}

static void cmd_help_handler0(uint32_t i)
{
	printf("%"CMD_MAX_NAME_LENGTH"s", cmd_array[i].name);
	const char *atp = cmd_array[i].arg_types;
	const char *adp = cmd_array[i].arg_dscr;
	int32_t req = strlen(atp) - cmd_array[i].arg_optional;
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
