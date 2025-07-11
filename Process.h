#pragma once
#include "ICommand.h"

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
#include <mutex>

//forward declare 
class PrintCommand;
/**
 * @class Process
 * @brief Represents a process running on the operating system.
 */
class Process {
	friend class PrintCommand;
public:
	Process(const std::string& name, int pid, int instructionCount);
	
	std::queue<std::unique_ptr<ICommand>> instructionQueue;
	std::vector<std::string> logs;
	
	void createInstructions();

	/**
	 * @brief Run one of the process's instructions.
	 */
	void executeInstruction();

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

	std::unique_ptr<ICommand> createCommand(int& remaining, int depth);

	/**
	 * @brief Get a variable from the symbol table.
	 * @param name Name of the variable.
	 * @return Value of variable.
	 */
	uint16_t getVar(std::string name);

	/**
	 * @brief Create a variable and store it in the symbol table.
	 * @param name Name of the variable.
	 * @param val Value of the variable.
	 */
	void setVar(std::string name, uint16_t val);

	void setFinished(bool value);

	void setCoreId(int id);
	int getCoreId() const;

	int getTotalLines() const;

private:
	std::string name; // name of the process
	int pid; // unique id assigned to process
	int totalInstructions; // number of instructions contained in process to execute.
	int executedInstructions = 0;

	std::string creationTimestamp;

	int coreId; // core that the process is running on.
	bool finished; // flag that tracks whether the process has finished execution.

	std::unordered_map<std::string, uint16_t> symbolTable; // maps variable names to their value

	mutable std::mutex mtx; // protects coreId and finished
};