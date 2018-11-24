#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "emci/cmd_parser.h"

static cmd_env_t env;

int main (int argc, char *argv[])
{
	printf("Emci started\n");
	cmd_main_loop(&env);
}
