@echo off

:start

del /Q emci.exe

gcc -Wall -g -O3 -c src/emci_arg.c          -I include -I . -o obj/emci_arg.o
gcc -Wall -g -O3 -c src/emci_parser.c       -I include -I . -o obj/emci_parser.o
gcc -Wall -g -O3 -c src/emci_std_handlers.c -I include -I . -o obj/emci_std_handlers.o

gcc -Wall -g -O3 -c emci_profile.c         -I include -I . -o obj/emci_profile.o
gcc -Wall -g -O3 -c demo_main.c            -I include -I . -o obj/demo_main.o

gcc obj/demo_main.o obj/emci_arg.o obj/emci_parser.o obj/emci_profile.o obj/emci_std_handlers.o -o emci.exe
emci.exe

pause
goto start
