cmake_minimum_required(VERSION 3.31)
project(csopesy-mco)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp
        shell.cpp
        shell.h "AConsole.h" "AConsole.cpp"   "ScreenCommands.h" "ScreenCommands.cpp" "ScreenList.cpp" "ScreenList.h" "Marquee.h" "Marquee.cpp")
