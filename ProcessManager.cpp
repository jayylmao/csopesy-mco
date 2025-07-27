#include "ProcessManager.h"
#include "Process.h"

int ProcessManager::nextPID = 1;

std::shared_ptr<Process> ProcessManager::getSharedProcess(std::string name)
{
	std::lock_guard<std::mutex> lock(processMutex);
	auto it = processTable.find(name);

	if (it == processTable.end()) {
		throw std::runtime_error("[!] Process not found");
	}

	return it->second;
}

void ProcessManager::createProcess(const std::string& name, int instructionCount, int mem)
{
	std::lock_guard<std::mutex> lock(processMutex);
	int pid = nextPID++;
	auto proc = std::make_shared<Process>(name, pid, instructionCount, mem);
	processTable.insert({ name, std::move(proc) });
}

void ProcessManager::createProcess(const std::string& name, std::vector<std::unique_ptr<ICommand>>&& instructions)
{
	std::lock_guard<std::mutex> lock(processMutex);
	int pid = nextPID++;
	auto proc = std::make_shared<Process>(name, pid, std::move(instructions));
	processTable.insert({ name, std::move(proc) });
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

void ProcessManager::setMinMemPerProc(int mem)
{
	this->minMemPerProc = mem;
}

void ProcessManager::setMaxMemPerProc(int mem)
{
	this->maxMemPerProc = mem;
}