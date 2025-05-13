//
// Created by jaymi on 13/05/2025.
//

#include "shell.h"

#include <iostream>

shell::shell()
{

}


void shell::prompt()
{
    std::cout << "Welcome to the command line: \n";
}


void shell::printHeader()
{
    std::cout << " _______ ___  ___  ___ ___ __ __\n"
                 "/ __(_-</ _ \\/ _ \\/ -_|_-</ // /\n"
                 "\\__/___/\\___/ .__/\\__/___/\\_, / \n"
                 "           /_/           /___/  \n";
}
