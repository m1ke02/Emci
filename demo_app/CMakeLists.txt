cmake_minimum_required(VERSION 3.0)
project(EmciDemoApp LANGUAGES C)
set(EmciLibraryPath ${EmciDemoApp_SOURCE_DIR}/..)
add_executable(EmciDemoAppExe
    demo_main.c
    emci_profile.c
    ${EmciLibraryPath}/src/emci_arg.c
    ${EmciLibraryPath}/src/emci_parser.c
    ${EmciLibraryPath}/src/emci_std_handlers.c
)
target_include_directories(EmciDemoAppExe PRIVATE
    ${EmciDemoApp_SOURCE_DIR}
    ${EmciLibraryPath}/include
)
