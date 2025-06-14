#pragma once
#include "ICommand.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

/**
 * @class Process
 * @brief Represents a process running on the operating system.
 */
class Process {
public:
	Process(const std::string& name, int pid, int instructionCount);

	/**
	 * @brief Run one of the process's instructions.
	 */
	void executeInstruction();

	/**
	 * @brief Get the number of remaining instructions in the process.
	 * @return Number of remaining instructions to be executed.
	 */
	int getRemainingInstructions();

	/**
	 * @brief Get the program's status to see if it has finished executing or not.
	 * @return Flag that checks if program execution is finished.
	 */
	bool hasFinished() const;

	/**
	 * @brief Get the program's current line.
	 * @return Number of current line being executed.
	 */
	int getCurrentLine();

	/**
	 * @brief Get the program's unique process ID.
	 * @return Process ID.
	 */
	int getPID();

private:
	std::string name; // name of the process
	int pid; // unique id assigned to process
	int totalInstructions; // number of instructions contained in process to execute.
	int remainingInstructions; // number of instructions left to execute.
};