cmake_minimum_required(VERSION 3.31)
project(csopesy-mco)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp
        shell.cpp
        shell.h 
         
        
        "AddCommand.h"
        "AddCommand.cpp"
        "ConfigManager.h"
        "ConfigManager.cpp"
         
         
        "DeclareCommand.cpp"
        "DeclareCommand.h"
        "FCFSScheduler.h"
        "FCFSScheduler.cpp"
        "ICommand.h"
        "ICommand.cpp"
        "IScheduler.h"
        "Marquee.h"
        "Marquee.cpp"
        "PrintCommand.cpp"
        "PrintCommand.h"
        "Process.h"
        "Process.cpp"
        "ProcessManager.h"
        "ProcessManager.cpp"
        "RoundRobinScheduler.h"
        "RoundRobinScheduler.cpp"
        "ScreenCommands.h"
        "ScreenCommands.cpp"
        "SleepCommand.cpp"
        "SleepCommand.h"
        "SubtractCommand.cpp"
        "SubtractCommand.h"
 "ForCommand.cpp" "ForCommand.h")
