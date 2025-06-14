#pragma once
#include "ICommand.h"
#include <string>

/**
 * @class PrintCommand
 * @brief Command for printing a string.
 */
class PrintCommand : public ICommand {
public:
	/**
	 * @brief Create a command to print a message.
	 */
	PrintCommand(std::string& msg);

	/**
	 * @brief Execute the command. Temporary: print to a text file.
	 */
	void execute();

private:
	std::string message;
};