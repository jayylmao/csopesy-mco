cmake_minimum_required(VERSION 3.31)
project(csopesy-mco)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp
        shell.cpp
        shell.h 
        "AConsole.h" 
        "AConsole.cpp"
        "ScreenCommands.h"
        "ScreenCommands.cpp"
        "ConsoleManager.h"
        "ConsoleManager.cpp"
        "Process.h"
        "Process.cpp"
        "ProcessManager.h"
        "ProcessManager.cpp"
        "PrintCommand.cpp"
        "PrintCommand.h"
        "FCFSScheduler.h"
        "FCFSScheduler.cpp"
        "PrintCommand.h"
        "PrintCommand.cpp"
        "ICommand.h"
        "ICommand.cpp"

)
