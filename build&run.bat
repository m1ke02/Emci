@echo off

:start

del /Q emci.exe
gcc -Wall -g -O3 -c emci_main.c -o obj/emci_main.o
gcc -Wall -g -O3 -c cmd_parser.c -o obj/cmd_parser.o
gcc -Wall -g -O3 -c cmd_profile.c -o obj/cmd_profile.o
gcc obj/emci_main.o obj/cmd_parser.o obj/cmd_profile.o -o emci.exe
emci.exe

pause
goto start
