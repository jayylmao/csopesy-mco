//
// Created by jaymi on 13/05/2025.
//

#ifndef SHELL_H
#define SHELL_H

/**
 * @class Shell
 * @brief Defines shell functionality such as initialization and user prompt.
 */
class Shell {
    public:
        Shell();

        /**
         * @brief Print header and initialize operating system.
         */
        void initialize();
        void screen();

        /**
         * @brief Display prompt, ask user for input, and call other methods to handle command.
         */
        void prompt();

        /**
         * @brief Print 'CSOPESY' header.
         */
        static void printHeader();

        /**
         * @brief Get the flag that indicates whether the system should shut down.
         * @return Boolean representing whether system should shut down.
         */
        bool getQuit();

        /**
         * @brief Set the flag for the system to shut down.
         */
        void setQuit();

        /**
         * @brief Get the flag that indicates whether the system is initialized.
         * @return Boolean representing whether system is initialized.
         */
        bool getInit();

        /**
         * @brief Set the flag that indicates system initialization.
         */
        void setInit();

    private:
        bool init = false; // System starts uninitialized at first.
        bool quit = false; // Check if user requests operating system to shut down.
};



#endif //SHELL_H
