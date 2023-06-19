# Emci

## Adding Emci to an existing project
Add the following sources:
- src/emci_arg.c
- src/emci_parser.c
- src/emci_std_handlers.c

Add the following include dirs:
- include/

Create custom configuration files:
- emci_config.h (Defines platform configuration)
- emci_profile.c (Contains app-specific command list and service handlers)
- emci_profile.h

(See demo_app for examples)

## Building demo application
    cd demo_app
    mkdir build
    cd build
    cmake ..
    make
