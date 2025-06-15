#include "Process.h"

Process::Process(const std::string& name, int pid, int instructionCount)
{
	this->name = name;
	this->pid = pid;
	this->totalInstructions = instructionCount;
	this->remainingInstructions = instructionCount;

	// store creation timestamp.
	auto now = std::chrono::system_clock::now();
	std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
	std::tm* time_info = std::localtime(&raw_time);

	std::ostringstream oss;
	oss << std::put_time(time_info, "%m/%d/%Y, %I:%M:%S %p");
	creationTimestamp = oss.str();
}

void Process::executeInstruction()
{
	remainingInstructions--;
}

int Process::getRemainingInstructions()
{
	return remainingInstructions;
}

bool Process::hasFinished() const
{
	return remainingInstructions == 0;
}

int Process::getCurrentLine()
{
	return this->totalInstructions - this->remainingInstructions;
}

int Process::getPID()
{
	return this->pid;
}

std::string Process::getName()
{
	return this->name;
}

std::string Process::getCreationTimestamp() {
	
	return this->creationTimestamp;
}