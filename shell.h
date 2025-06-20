#pragma once

#include "AConsole.h"
#include "ScreenCommands.h"
#include "ConsoleManager.h"
#include "ProcessManager.h"
#include "FCFSScheduler.h"
#include "RoundRobinScheduler.h"
#include "ConfigManager.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>

/**
 * @class Shell
 * @brief Defines shell functionality such as initialization and user prompt.
 */
class Shell {
public:
    Shell(int cores);

    /**
     * @brief Print header and initialize operating system.
     */
    void initialize();

    /**
     * @brief Create a new process or list running processes.
     * @param args Perform different actions based on the passed argument. -s creates a new process, -r redraws the console screen before creating a new process, and -ls lists all running processes
     */
    void screen(std::vector<std::string> args);

    /**
     * @brief List all running processes.
     */
    void screenList();

    /**
     * @brief Continuously create new process instances until the user calls the stop command.
     */
    void schedulerStart();

    /**
     * @brief Stop the creation of new process instances.
     */
    void schedulerStop();

    /**
     * @brief Display a report of CPU usage and progress.
     */
    void reportUtil();

    /**
     * @brief Splits a given input string given a delimiter and returns a vector of tokens.
     * @param string Input string to split.
     * @param delim Split string on delimiter character.
     * @param &tokens Reference to token vector to push tokens to.
     */
    static void splitString(std::string const& string, char const delim, std::vector<std::string>& tokens);

    /**
     * @brief Display prompt, ask user for input, and call other methods to handle command.
     */
    void prompt();

    /**
     * @brief Print 'CSOPESY' header.
     */
    static void printHeader();

    /**
     * @brief Clear the console screen and reprint the header.
     */
    static void clear();

    /**
     * @brief Exit the focused program. If no program is active, exit the operating system.
     */
    void exit();

    /**
     * @brief Get the flag that indicates whether the system should shut down.
     * @return Boolean representing whether system should shut down.
     */
    bool getQuit() const;

    /**
     * @brief Set the quit flag and shut down the system.
     */
    void setQuit();

    /**
     * @brief Get the flag that indicates whether the system is initialized.
     * @return Boolean representing whether system is initialized.
     */
    bool getInit() const;

    /**
     * @brief Set the flag that indicates system initialization.
     */
    void setInit();

    /**
     * @brief Get the ID of the focused process.
     * @return ID of the focused process.
     */
    int getFocusedPID() const;

    /**
     * @brief Set a new process as focused.
     * @param pid ID to set focus to.
     */
    void setFocusedPID(int pid);

    /**
     * @brief Get the number of cores.
     */
    int getCores();

private:
    bool init; // System starts uninitialized at first.
    bool quit; // Check if user requests operating system to shut down.
    int focusedPID; // ID of focused process. The OS main menu is 0.
    int cores;

    int nextPID = 1;

    RoundRobinScheduler scheduler;
    ProcessManager processManager;
    ConfigManager configManager;
};