#include "emci_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

//	external application-dependent functions
void cmd_prompt(void);
const char *cmd_app_status_message(cmd_status_t status);

//	external application-dependent variables
extern const cmd_command_t cmd_array[];
extern const uint_fast8_t cmd_array_length;

static void cmd_response_handler(uint8_t argc, char *raw_argv[], cmd_response_t *resp);

void cmd_main_loop(cmd_env_t *env)
{
	int c;
	char *cursor;
	char *tokens[CMD_MAX_COMMANDS];

	while (1)
	{
		// prepare to command line parsing
		cursor = env->cmd_buffer;
		*cursor = '\0'; // start from empty line
		cmd_prompt();

		// read command line char by char
		while (1)
		{
			if ((c = fgetc(stdin)) == EOF)
				return;
			
			// convert several special characters
			if (c == '\t')
				c = ' ';	// replace tabs with spaces
			else if (c == '\x1B')
				c = '^';	// display escape sequences
			else if (c == '\x7F')
			{
				// backspace key support for terminal
				if (cursor > env->cmd_buffer)
				{
					cursor --;
					#if CMD_ECHO_INPUT == 1
					putc('\x7F', stdout);
					#endif
				}
				continue;
			}

			if (c == CMD_ENTER_KEY)
			{
				// terminate last command
				*cursor = '\0';

				#if CMD_ECHO_INPUT == 1
				printf(CMD_ENDL);
				#endif

				uint_fast8_t ntokens = cmd_tokenize(env->cmd_buffer, CMD_COMMAND_DEL, true,
					tokens, sizeof(tokens) / sizeof(tokens[0]));

				// run command handler
				for (uint_fast8_t i = 0; i < ntokens; i ++)
				{
					//printf("cmd%d=[%s] CMD_ENDL", i, tokens[i]);
					cmd_process_command(tokens[i], env);
				}

				// we are done, go to next line
				break;
			}
			else if (cursor - env->cmd_buffer < CMD_MAX_LINE_LENGTH)
			{
				*cursor++ = c;

				#if CMD_ECHO_INPUT == 1
				putc(c, stdout);
				#endif
			}
		}
	}
}

void cmd_process_command(char *command, cmd_env_t *env)
{
	char *tokens[CMD_MAX_ARGS+1]; // plus 1 for command name

	// get tokens for command & args
	uint_fast8_t ntokens = cmd_tokenize(command, CMD_ARG_DEL, false,
		tokens, sizeof(tokens) / sizeof(tokens[0]));

	/*for (uint_fast8_t i = 0; i < ntokens; i ++)
	{
		printf("   arg%d=[%s]" CMD_ENDL, i, tokens[i]);
	}*/

	if (ntokens > 0)
	{
		env->resp.status = CMD_STATUS_OK;
		env->resp.msg = NULL;
		env->resp.param = 0;

		// search for requested command
		uint_fast8_t cmd_num;
		for (cmd_num = 0; cmd_num < cmd_array_length; cmd_num ++)
			if (strcmp(cmd_array[cmd_num].name, tokens[0]) == 0)
				break;
		if (cmd_num >= cmd_array_length)
			env->resp.status = CMD_STATUS_UNKNOWN_CMD;

		if (env->resp.status == CMD_STATUS_OK)
		{
			// command found
			env->cmd = (cmd_command_t *)&(cmd_array[cmd_num]);
			int_fast8_t nargs_max = strlen(cmd_array[cmd_num].arg_types);
			int_fast8_t nargs_min = nargs_max - cmd_array[cmd_num].arg_optional;

			// check if arg specification is valid
			if (nargs_max > CMD_MAX_ARGS || nargs_min < 0)
			{
				env->resp.param = cmd_num;
				env->resp.msg = "Argument count";
				env->resp.status = CMD_STATUS_PROFILE_ERROR;
			}

			// check argument count
			if (ntokens < nargs_min+1)
			{
				env->resp.param = nargs_min; // set to nearest correct argument count
				env->resp.status = CMD_STATUS_ARG_TOO_FEW;
			}
			else if (ntokens > nargs_max+1)
			{
				env->resp.param = nargs_max; // set to nearest correct argument count
				env->resp.status = CMD_STATUS_ARG_TOO_MANY;
			}

			if (env->resp.status == CMD_STATUS_OK)
			{
				// convert name & args from strings to cmd_arg_t variants
				cmd_arg_convert(tokens[0], CMD_ARG_STRING, &(env->arg_buffer[0]));
				for (uint_fast8_t t = 1; t < ntokens; t ++)
				{
					env->resp.status = cmd_arg_convert(
						tokens[t], 
						(cmd_arg_type_t)cmd_array[cmd_num].arg_types[t-1], 
						&(env->arg_buffer[t])
					);
					if (env->resp.status != CMD_STATUS_OK)
					{
						env->resp.param = t;
						break;
					}
				}

				if (env->resp.status == CMD_STATUS_OK)
				{
					// call the command handler
					env->resp.status = cmd_array[cmd_num].handler(ntokens, env->arg_buffer, env);
				}
			}
		}

		cmd_response_handler(ntokens, tokens, &(env->resp));
	}
}

static void cmd_response_handler(uint8_t argc, char *raw_argv[], cmd_response_t *resp)
{
	printf("~%s %d (%s", raw_argv[0], resp->status, cmd_status_message(resp->status));

	switch (resp->status)
	{
		case CMD_STATUS_ARG_TOO_MANY:
		case CMD_STATUS_ARG_TOO_FEW:
			// param is closest valid number of arguments
			printf(": required %u but passed %u)" CMD_ENDL, resp->param, argc-1);
			break;
		case CMD_STATUS_ARG_FORMAT:
		case CMD_STATUS_ARG_INVALID:
		case CMD_STATUS_ARG_TOO_HIGH:
		case CMD_STATUS_ARG_TOO_LOW:
			// param is number of invalid argument
			printf(": %s)" CMD_ENDL, raw_argv[resp->param]);
			break;
		case CMD_STATUS_OK:
		case CMD_STATUS_UNKNOWN_CMD:
			printf(")" CMD_ENDL);
			break;
		case CMD_STATUS_PROFILE_ERROR:
		default:
			if (resp->msg)
				printf(": %s)" CMD_ENDL, resp->msg);
			else
				printf(")" CMD_ENDL);
			break;
	}
}

const char *cmd_status_message(cmd_status_t status)
{
	switch (status)
	{
		case CMD_STATUS_OK: return "OK";
		case CMD_STATUS_UNKNOWN_CMD: return "Unknown command";
		case CMD_STATUS_PROFILE_ERROR: return "Profile error";
		case CMD_STATUS_ARG_TOO_MANY: return "Too many arguments";
		case CMD_STATUS_ARG_TOO_FEW: return "Too few arguments";
		case CMD_STATUS_ARG_FORMAT: return "Incorrect argument format";
		case CMD_STATUS_ARG_INVALID: return "Invalid argument";
		case CMD_STATUS_ARG_TOO_LOW: return "Parameter value too low";
		case CMD_STATUS_ARG_TOO_HIGH: return "Parameter value too high";
		default: 
			if (status >= CMD_STATUS_APP_ERROR_START && status <= CMD_STATUS_APP_ERROR_END)
				return cmd_app_status_message(status);
			else
				return "?";
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
