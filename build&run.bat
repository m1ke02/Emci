@echo off

:start

del /Q emci.exe

gcc -Wall -g -O3 -c demo_main.c -o obj/demo_main.o
gcc -Wall -g -O3 -c cmd_profile.c -o obj/cmd_profile.o

gcc -Wall -g -O3 -c emci/cmd_arg.c -o obj/cmd_arg.o
gcc -Wall -g -O3 -c emci/cmd_parser.c -o obj/cmd_parser.o
gcc -Wall -g -O3 -c emci/cmd_std_handlers.c -o obj/cmd_std_handlers.o
gcc obj/demo_main.o obj/cmd_arg.o obj/cmd_parser.o obj/cmd_profile.o obj/cmd_std_handlers.o -o emci.exe
emci.exe

pause
goto start
