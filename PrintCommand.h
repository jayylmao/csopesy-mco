#pragma once
#include "ICommand.h"
#include <string>
#include <iostream>
#include <fstream>

/**
 * @class PrintCommand
 * @brief Command for printing a string.
 */
class PrintCommand : public ICommand {
public:
	/**
	 * @brief Create a command to print a message.
	 */
	PrintCommand(const std::string& msg, int coreId, const std::string& procName, int pid);


	/**
	 * @brief Execute the command. Temporary: print to a text file.
	 */
	void execute() override;
	/**
	*  @brief Sets the coreID for each instruction based on the core executing it.
	*/
	void setCoreID(int core) { coreID = core; }
private:
	std::string message;
	int coreId;              
	std::string procName;    
};