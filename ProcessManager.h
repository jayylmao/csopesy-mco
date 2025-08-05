#pragma once
#include <map>
#include <vector>
#include <mutex>
#include <stdexcept>
#include "DemandPagingAllocator.h"

class ICommand;
class Process;

/**
 * @class ProcessManager
 * @brief Handles starting, tracking, and killing processes.
 */
class ProcessManager {
public:
	ProcessManager(std::shared_ptr<DemandPagingAllocator> memoryManager);

	// create type definition for table that tracks processes.
	typedef std::map<std::string, std::shared_ptr<Process>> ProcessTable;

	/**
	 * @brief List all processes.
	 * @return Vector containing pointers to all processes.
	 */
	std::vector<Process*> listProcesses();
	
	/**
	 * @brief Get a shared pointer to a process from the process manager through its name.
	 * @return Shared pointer to a process of a given name.
	 */
	std::shared_ptr<Process> getSharedProcess(std::string name);

	/**
	 * @brief Sets the minimum amount of memory each process needs.
	 */
	void setMinMemPerProc(int mem);
	
	/**
	 * @brief Sets the maximum amount of memory each process needs.
	 */
	void setMaxMemPerProc(int mem);

	int getNextPID();

	/**
	 * @brief Create a process instance.
	 * @param name Name of process.
	 * @param pid Unique ID of process.
	 * @param instructionCount Number of instructions that process has.
	 */
	void createProcess(const std::string& name, int instructionCount, int mem, size_t pageSize);

	void createProcess(const std::string& name, std::vector<std::unique_ptr<ICommand>>&& instructions, size_t pageSize);

private:
	ProcessTable processTable;
	std::mutex processMutex;
	static int nextPID;
	int minMemPerProc;
	int maxMemPerProc;

	std::shared_ptr<DemandPagingAllocator> memoryManager;
};