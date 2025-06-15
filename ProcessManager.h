#pragma once
#include "Process.h"
#include <map>
#include <vector>
#include <mutex>
#include <stdexcept>

/**
 * @class ProcessManager
 * @brief Handles starting, tracking, and killing processes.
 */
class ProcessManager {
public:
	// create type definition for table that tracks processes.
	typedef std::map<int, std::shared_ptr<Process>> ProcessTable;

	/**
	 * @brief List all processes.
	 * @return Vector containing pointers to all processes.
	 */
	std::vector<Process*> listProcesses();

	/**
	 * @brief Get a reference to a process from its unique ID.
	 */
	Process& getProcess(int pid);

	/**	
	 * @brief Get a process's name from its unique ID.
	 */
	std::string getProcessName(int pid);
	
	/**
	 * @brief Get a shared pointer to a process from the process manager through its unique ID.
	 */
	std::shared_ptr<Process> getSharedProcess(int pid);

	int getNextPID();

	/**
	 * @brief Create a process instance.
	 * @param name Name of process.
	 * @param pid Unique ID of process.
	 * @param instructionCount Number of instructions that process has.
	 */
	void createProcess(const std::string& name, int instructionCount);

private:
	ProcessTable processTable;
	std::mutex processMutex;
	static int nextPID;

};