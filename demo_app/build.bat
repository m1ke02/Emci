@echo off

pushd build

set EMCI_ROOT=../..

rem Build common files
gcc -Wall -g -O3 -c %EMCI_ROOT%/src/emci_arg.c          -I %EMCI_ROOT%/include -I .. -o emci_arg.o
gcc -Wall -g -O3 -c %EMCI_ROOT%/src/emci_parser.c       -I %EMCI_ROOT%/include -I .. -o emci_parser.o
gcc -Wall -g -O3 -c %EMCI_ROOT%/src/emci_std_handlers.c -I %EMCI_ROOT%/include -I .. -o emci_std_handlers.o
rem Build local project files
gcc -Wall -g -O3 -c ../emci_profile.c         -I %EMCI_ROOT%/include -I .. -o emci_profile.o
gcc -Wall -g -O3 -c ../demo_main.c            -I %EMCI_ROOT%/include -I .. -o demo_main.o
rem Link
gcc demo_main.o emci_arg.o emci_parser.o emci_profile.o emci_std_handlers.o -o emci.exe

popd
