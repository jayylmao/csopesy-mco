#pragma once
#include "ICommand.h"
#include "PrintCommand.h"
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "SubtractCommand.h"
#include "SleepCommand.h"
#include "ForCommand.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <chrono>
#include <queue>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <iomanip>  // for std::put_time
#include <sstream>  // for std::ostringstream
#include <cstdint>


#include <cstdlib>  // for rand()
#include <ctime>    // for seeding rand() if needed
#include <iterator> // for std::advance
#include <random>
#include <cstdint>

/**
 * @class Process
 * @brief Represents a process running on the operating system.
 */
class Process {
public:
	Process(const std::string& name, int pid, int instructionCount);
	
	std::queue<std::shared_ptr<ICommand>> instructionQueue;
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

	std::shared_ptr<ICommand> createCommand(int type);

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

	std::string creationTimestamp;

	int coreId; // core that the process is running on.
	bool finished; // flag that tracks whether the process has finished execution.

	std::unordered_map<std::string, uint16_t> symbolTable; // maps variable names to their value

	mutable std::mutex mtx; // protects coreId and finished
};