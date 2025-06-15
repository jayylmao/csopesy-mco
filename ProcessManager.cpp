#include "ProcessManager.h"

int ProcessManager::nextPID = 1;

Process& ProcessManager::getProcess(int pid)
{
	std::lock_guard<std::mutex> lock(processMutex);
	auto it = processTable.find(pid);
	if (it == processTable.end()) {
		throw std::runtime_error("Process not found");
	}
	return *(it->second);
}

std::string ProcessManager::getProcessName(int pid)
{
	std::lock_guard<std::mutex> lock(processMutex);
	return getProcess(pid).getName();
}

std::shared_ptr<Process> ProcessManager::getSharedProcess(int pid)
{
	std::lock_guard<std::mutex> lock(processMutex);
	auto it = processTable.find(pid);

	if (it == processTable.end()) {
		throw std::runtime_error("[!] Process not found");
	}

	return it->second;
}

void ProcessManager::createProcess(const std::string& name, int instructionCount)
{
	std::lock_guard<std::mutex> lock(processMutex);
	int pid = nextPID++;
	auto proc = std::make_shared<Process>(name, pid, instructionCount);
	processTable.insert({ pid, std::move(proc) });
}

std::vector<Process*> ProcessManager::listProcesses()
{
	std::lock_guard<std::mutex> lock(processMutex);

	std::vector<Process*> processes;
	for (auto& [pid, proc] : processTable) {
		processes.push_back(proc.get());
	}

	return processes;
}

int ProcessManager::getNextPID()
{
	return this->nextPID;
}