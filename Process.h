#pragma once
#include "ICommand.h"
#include "DemandPagingAllocator.h"

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
	Process(const std::string& name, int pid, int instructionCount, int mem, size_t pageSize, std::shared_ptr<DemandPagingAllocator> memoryManager);

	Process(const std::string& name, int pid, std::vector<std::unique_ptr<ICommand>>&& instructions, size_t pageSize, std::shared_ptr<DemandPagingAllocator> memoryManager);
	
	std::queue<std::unique_ptr<ICommand>> instructionQueue;
	std::vector<std::string> logs;
	
	void createInstructions();

	/**
	 * @brief Helper to recursively flatten FOR-like logic into instructionQueue.
	 */
	void createFlatCommand(int& remaining, int depth);
	/**
	 * @brief Sets the process's instructions from pre-parsed command components.
	 * @param instructions A vector of instructions, where each instruction is broken down
	 *               into its command and arguments (e.g., {"DECLARE", "varA", "10"}).
	 */
	void setParsedInstructions(const std::vector<std::vector<std::string>>& instructions);

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

	int getMemory() const;

private:
	std::string name; // name of the process
	int pid; // unique id assigned to process
	int totalInstructions; // number of instructions contained in process to execute.
	int executedInstructions = 0;

	std::string creationTimestamp;

	int coreId; // core that the process is running on.
	bool finished; // flag that tracks whether the process has finished execution.

	int memory; // amount of memory the process used.

	size_t numPages; // number of pages accessible to the memory.

	std::unordered_map<std::string, uint16_t> symbolTable; // maps variable names to their value

	mutable std::mutex mtx; // protects coreId and finished

	std::shared_ptr<DemandPagingAllocator> memoryManager;

	/**
	 * @brief Helper method to access page in memory.
	 */
	void accessCurrentPage();
};