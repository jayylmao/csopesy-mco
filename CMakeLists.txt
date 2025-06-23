cmake_minimum_required(VERSION 3.31)
project(csopesy-mco)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp
        shell.cpp
        shell.h 
        "AConsole.h" 
        "AConsole.cpp"
        "ConfigManager.h"
        "ConfigManager.cpp"
        "ConsoleManager.cpp" 
        "ConsoleManager.h" 
        "FCFSScheduler.h"
        "FCFSScheduler.cpp"
        "ICommand.h"
        "ICommand.cpp"
        "PrintCommand.h"
        "PrintCommand.cpp"
        "Process.h"
        "Process.cpp"
        "ProcessManager.h"
        "ProcessManager.cpp"
        "RoundRobinScheduler.h"
        "RoundRobinScheduler.cpp"
        "ScreenCommands.h"
        "ScreenCommands.cpp"
        "IScheduler.h"
)
