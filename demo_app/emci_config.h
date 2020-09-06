#ifndef EMCI_CONFIG_H
#define EMCI_CONFIG_H

#define CMD_COMMAND_DEL			';'
#define CMD_ARG_DEL				' '
#define CMD_ENTER_KEY			'\n'
#define CMD_ENDL				"\r\n"
#define CMD_ECHO_INPUT			0
#define CMD_MAX_LINE_LENGTH		32
#define CMD_MAX_COMMANDS		8
#define CMD_MAX_ARGS			10		// see "if (!adp)" line inside cmd_help_handler()
#define CMD_MAX_NAME_LENGTH		"10"	// note the quotes

#endif
