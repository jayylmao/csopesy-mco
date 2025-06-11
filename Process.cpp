#include "Process.h"

Process::Process(const std::string& name, int pid, int instructionCount)
{
	this->name = name;
	this->pid = pid;
	this->totalInstructions = instructionCount;
	this->remainingInstructions = instructionCount;
}

