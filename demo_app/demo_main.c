#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "emci_parser.h"

static emci_env_t env;

int main (int argc, char *argv[])
{
	printf("Emci started\n");
	emci_main_loop(&env);
}
