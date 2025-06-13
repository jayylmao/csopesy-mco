#pragma once
#include <iostream>
#include <string>
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
	 */
	int getRemainingInstructions();

	/**
	 * @brief Get the program's status to see if it has finished executing or not.
	 */
	bool hasFinished() const;

private:
	std::string name; // name of the process
	int pid; // unique id assigned to process
	int totalInstructions; // number of instructions contained in process to execute.
	int remainingInstructions; // number of instructions left to execute.
};