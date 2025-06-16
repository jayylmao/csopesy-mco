#pragma once
#include "ICommand.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <chrono>

#include <mutex>
#include <iomanip>  // for std::put_time
#include <sstream>  // for std::ostringstream

/**
 * @class Process
 * @brief Represents a process running on the operating system.
 */
class Process {
public:



	Process(const std::string& name, int pid, int instructionCount);

	/**
	 * @brief Get the total number of instructions.
	 * @return Total instruction count.
	 */
	int getTotalInstructions() const;

	/**
	 * @brief Add a command (instruction) to this process.
	 * @param cmd A shared pointer to an ICommand object.
	 */
	void addCommand(std::shared_ptr<ICommand> cmd);

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

	/**
	 * @brief Get the program's name.
	 * @return Program's name.
	 */
	std::string getName();

	/**
	 * @brief Get the time of the process's creation.
	 * @return String of process's formatted timestamp.
	 */
	std::string getCreationTimestamp();
	/**
	 * @brief Get a pointer to the current instruction to be executed.
	 * @return Pointer to the current ICommand object.
	 */
	ICommand* getCurrentInstruction() {
		return instructions[currentInstructionIndex].get();
	}
	/**
	 * @brief Setter and getter for cores
	 */
	int getAssignedCore() const;
	void setAssignedCore(int coreId);


	void setFinished(bool value);

	void setCoreId(int id);
	int getCoreId() const;

	int getTotalLines() const;


private:
	std::string name; // name of the process
	int pid; // unique id assigned to process
	int totalInstructions; // number of instructions contained in process to execute.
	int remainingInstructions; // number of instructions left to execute.
	int currentInstructionIndex = 0;

	int assignedCore = 0; //unassiagned core ID SET TO 0

	std::string creationTimestamp;

	int coreId;
	bool finished;

	mutable std::mutex mtx; // protects coreId and finished
};