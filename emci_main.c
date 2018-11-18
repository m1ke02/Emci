#include <stdint.h>
#include <stdio.h>
#include "cmd_parser.h"

#include <stdlib.h>

static cmd_env_t env;

int main (int argc, char *argv[])
{
	//char str[180] = "This_is 14 ; strdisplay nahREN!!! ZZ$$  VV%% BB^^ ;  about ; push  raz dva ;  test  600 ;; setdelay 12 13 ; setdelay 12345 -3.141e3 -600 true ; help";
	
	printf("Emci started\n");
	cmd_main_loop(&env);
}
