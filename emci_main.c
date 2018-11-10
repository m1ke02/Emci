#include <stdint.h>
#include <stdio.h>
#include "cmd_parser.h"

#include <stdlib.h>

int main (int argc, char *argv[])
{
	//char str[180] = "This_is 14 ; strdisplay nahREN!!! ZZ$$  VV%% BB^^ ;  about ; push  raz dva ;  test  600 ;; setdelay 12 13 ; setdelay 12345 -3.141e3 -600 true ; help";
	//cmd_process_line(str);
	
	cmd_main_loop();

	/*float x = 0;
	char *data = " -333E+3";
	cmd_status_t status = cmd_strtof2(data, &x);
	printf("res=%f status=%s\r\n", x, cmd_status_message(status));*/
}

int _write(int fd, char * str, int len)
{
	return 0;
}

struct __FILE
{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};

/* FILE is typedef’d in stdio.h. */
FILE __stdout;

int fputc(int ch, FILE *f)
{
  /* Your implementation of fputc(). */
  return ch;
}

int ferror(FILE *f)
{
  /* Your implementation of ferror(). */
  return 0;
}
