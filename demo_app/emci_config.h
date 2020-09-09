#ifndef EMCI_CONFIG_H
#define EMCI_CONFIG_H

#define EMCI_COMMAND_DEL        ';'
#define EMCI_ARG_DEL            ' '
#define EMCI_ENTER_KEY          '\n'
#define EMCI_ENDL               "\r\n"
#define EMCI_ECHO_INPUT         0
#define EMCI_MAX_LINE_LENGTH    32
#define EMCI_MAX_COMMANDS       8
#define EMCI_MAX_ARGS           10    // see "if (!adp)" line inside cmd_help_handler()
#define EMCI_MAX_NAME_LENGTH    "10"  // note the quotes

#endif
