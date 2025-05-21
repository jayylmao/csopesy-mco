//
// Created by jaymi on 13/05/2025.
//

#include "shell.h"

#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>

Shell::Shell()
{
    init = false;
    quit = false;
    focusedPID = 0;
}

bool Shell::getQuit() const
{
    return quit;
}

void Shell::setQuit()
{
    quit = true;
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

int Shell::getFocusedPID() const
{
    return focusedPID;
}

void Shell::setFocusedPID(const int pid)
{
    focusedPID = pid;
}

void Shell::initialize()
{
    if (!init) {
        init = true;
        printHeader();
        prompt();
    } else {
        std::cout << "[i] initialize command recognized. Doing something." << std::endl;
    }
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
    std::cout << "[i] clear command recognized. Doing something." << std::endl;
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
    
    printHeader();
}

void Shell::exit()
{
    if (getFocusedPID() == 0) {
        setQuit();
    } else { // TODO: exit process. for now, this just sets the focused pid and nothing else.
        setFocusedPID(0);
    }
}

void Shell::printHeader()
{
    std::cout << " _______ ___  ___  ___ ___ __ __\n"
                 "/ __(_-</ _ \\/ _ \\/ -_|_-</ // /\n"
                 "\\__/___/\\___/ .__/\\__/___/\\_, / \n"
                 "           /_/           /___/  \n"
                 "Welcome to the CSOPESY command line.\n";
}

void Shell::splitString(std::string const &string, char const delim, std::vector<std::string> &tokens)
{
    // push an empty string and return to avoid going through the splitting process if input is empty.
    if (string.empty()) {
        tokens.push_back("");
        return;
    }

    size_t start;
    size_t end = 0;

    // find start and end of non-delimiter substrings and push them to a vector.
    while ((start = string.find_first_not_of(delim, end)) != std::string::npos) {
        end = string.find(delim, start);
        tokens.push_back(string.substr(start, end - start));
    }
}

void Shell::prompt()
{
    std::string input;
    constexpr char delimiter = ' '; // command arguments are separated by a space.

    std::cout << "user ~ > " << std::flush;
    std::getline(std::cin, input);

    // don't perform string split when input is empty.
    if (input.empty()) {
        return;
    }

    // split string to get command and parameters as vector.
    std::vector<std::string> input_tokens;
    splitString(input, delimiter, input_tokens);

    if (input_tokens[0] == "initialize") {
        initialize();
    } else if (input_tokens[0] == "screen") {
        screen();
    } else if (input_tokens[0] == "scheduler-test") {
        schedulerTest();
    } else if (input_tokens[0] == "scheduler-stop") {
        schedulerStop();
    } else if (input_tokens[0] == "report-util") {
        reportUtil();
    } else if (input_tokens[0] == "clear") {
        clear();
    } else if (input_tokens[0] == "exit") {
        setQuit();
    } else if (!input_tokens[0].empty()) { // error on unknown, non-empty command.
        std::cout << "[*] Unknown command. Type 'help' to get a list of commands." << std::endl;
    }
}
