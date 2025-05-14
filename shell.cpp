//
// Created by jaymi on 13/05/2025.
//

#include "shell.h"

#include <iostream>
#include <string>

Shell::Shell()
{

}

void Shell::initialize()
{
    printHeader();
    prompt();
}

void Shell::screen()
{
    std::cout << "[i] screen command recognized. Doing something." << std::endl;
}

void Shell::schedulerTest()
{
    std::cout << "[i] scheduler-test command recognized. Doing something." << std::endl;
}

void Shell::schedulerStop()
{
    std::cout << "[i] scheduler-stop command recognized. Doing something." << std::endl;
}

void Shell::reportUtil()
{
    std::cout << "[i] report-util command recognized. Doing something." << std::endl;
}

void Shell::clear()
{
    // std::cout << "\033[2J\033[1;1H" << std::endl;
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
    
    printHeader();
}

void Shell::printHeader()
{
    std::cout << " _______ ___  ___  ___ ___ __ __\n"
                 "/ __(_-</ _ \\/ _ \\/ -_|_-</ // /\n"
                 "\\__/___/\\___/ .__/\\__/___/\\_, / \n"
                 "           /_/           /___/  \n"
                 "Welcome to the CSOPESY command line.\n";
}

void Shell::prompt()
{
    std::string input;

    std::cout << "user ~ > " << std::flush;
    std::getline(std::cin, input);

    if (input == "initialize") {
        initialize();
    } else if (input == "screen") {
        screen();
    } else if (input == "scheduler-test") {
        schedulerTest();
    } else if (input == "scheduler-stop") {
        schedulerStop();
    } else if (input == "report-util") {
        reportUtil();
    } else if (input == "clear") {
        clear();
    } else if (input == "exit") {
        setQuit();
    } else if (!input.empty()) { // error on unknown, non-empty command.
        std::cout << "[*] Unknown command. Type 'help' to get a list of commands." << std::endl;
    }
}

bool Shell::getQuit() const
{
    return quit;
}

void Shell::setQuit()
{
    quit = true;

    std::cout << "[i] Shutting down command line. Goodbye." << std::endl;
    std::exit(EXIT_SUCCESS);
}

bool Shell::getInit() const
{
    return init;
}

void Shell::setInit()
{
    init = true;
}
