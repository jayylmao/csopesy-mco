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
    } else if (input == "quit") {
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
