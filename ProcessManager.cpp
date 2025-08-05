#include "ProcessManager.h"
#include "Process.h"
#include "DemandPagingAllocator.h"

int ProcessManager::nextPID = 1;

ProcessManager::ProcessManager(std::shared_ptr<DemandPagingAllocator> memoryManager)
	: memoryManager(memoryManager)
{}

std::shared_ptr<Process> ProcessManager::getSharedProcess(std::string name)
{
	std::lock_guard<std::mutex> lock(processMutex);
	auto it = processTable.find(name);

	if (it == processTable.end()) {
		throw std::runtime_error("[!] Process not found");
	}

	return it->second;
}

void ProcessManager::createProcess(const std::string& name, int instructionCount, int mem, size_t pageSize)
{
	std::lock_guard<std::mutex> lock(processMutex);
	int pid = nextPID++;
	memoryManager->allocate(mem, pid);
	auto proc = std::make_shared<Process>(name, pid, instructionCount, mem, pageSize, memoryManager);
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