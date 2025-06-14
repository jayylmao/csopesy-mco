#pragma once
#include "Process.h"

/**
 * @class FCFSScheduler
 * @brief Process scheduler that follows first-come-first-serve.
 */
class FCFSScheduler {
public:
	/**
	 * @brief Instantiate scheduler with given number of cores.
	 * @param cores Number of cores for scheduler to use.
	 */
	FCFSScheduler(int cores) : numCores(cores), processQueues(cores);

	/**
	 * @brief Add a process to the queue.
	 * @param process Reference to a process to add to the queue.
	 * @param core Core index to run process on.
	 */
	void addProcess(const Process& process, int core = 0);

	/**
	 * @brief Run the scheduler.
	 */
	void runScheduler();

private:
	int numCores;
	std::vector<std::vector<Process>> processQueues; // Store process queues per core.
};