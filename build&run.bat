@echo off

:start

del /Q emci.exe

gcc -Wall -g -O3 -c src/cmd_arg.c          -I include -o obj/cmd_arg.o
gcc -Wall -g -O3 -c src/cmd_parser.c       -I include -o obj/cmd_parser.o
gcc -Wall -g -O3 -c src/cmd_std_handlers.c -I include -o obj/cmd_std_handlers.o

gcc -Wall -g -O3 -c emci_profile.c         -I include -o obj/emci_profile.o
gcc -Wall -g -O3 -c demo_main.c            -I include -o obj/demo_main.o

gcc obj/demo_main.o obj/cmd_arg.o obj/cmd_parser.o obj/emci_profile.o obj/cmd_std_handlers.o -o emci.exe
emci.exe

pause
goto start
