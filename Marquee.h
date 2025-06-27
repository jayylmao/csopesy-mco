#pragma once
#include <cstdlib>
#include <string>
#include <vector>

#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <conio.h>

class Marquee {
public:
    Marquee();

    /**
     * @brief Print header and initialize operating system.
     */
    void initialize();

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
    * @brief Runs the bouncing marquee text animation.
    */
    void runMarquee();

    /**
     * @brief Clear the console screen and reprint the header.
     */
    static void clear();

    /**
     * @brief Get the flag that indicates whether the system should shut down.
     * @return Boolean representing whether system should shut down.
     */
    bool getQuit() const;

    /**
     * @brief Set the quit flag and shut down the system.
     */
    void setQuit();


private:
    bool init; // System starts uninitialized at first.
    bool quit; // Check if user requests operating system to shut down.
};


