#include "FCFSScheduler.h"

void FCFSScheduler::addProcess(const Process& process, int core = 0)
{
	if (core >= 0 && core < numCores) {
		processQueues[core].push_back(process);
	}
	else {
		std::cerr << "[!] Invalid core selected." << std::endl;
	}
}

void FCFSScheduler::runScheduler()
{
	// while the first queue isn't empty, iterate through each queue
	while (!processQueues[0].empty()) {
		for (int core = 0; core < numCores; core++) {
			// if the queue isn't empty, execute the first process in the queue.
			if (!processQueues[core].empty()) {
				Process currProcess = processQueues[core].back();
				processQueues[core].pop_back();

				// while the process hasn't finished, execute all instructions.
				while (!currProcess.hasFinished()) {
					currProcess.executeInstruction();
				}

				std::cout << "Process " << currProcess.getPID() << " has completed on core" << core + 1 << "." << std::endl;
			}
		}
	}
}