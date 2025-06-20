#include "Process.h"

Process::Process(const std::string& name, int pid, int instructionCount)
	: name(name), pid(pid), totalInstructions(instructionCount),
	remainingInstructions(instructionCount), coreId(-1), finished(false)
{
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
	std::lock_guard<std::mutex> lock(mtx);
	if (remainingInstructions > 0) {
		--remainingInstructions;
		if (remainingInstructions == 0) {
			finished = true;
		}
	}
}

int Process::getRemainingInstructions()
{
	std::lock_guard<std::mutex> lock(mtx);
	return remainingInstructions;
}

bool Process::hasFinished() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return finished;
}

void Process::setFinished(bool value)
{
	std::lock_guard<std::mutex> lock(mtx);
	finished = value;
}

int Process::getCurrentLine()
{
	std::lock_guard<std::mutex> lock(mtx);
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

void Process::setCoreId(int id)
{
	std::lock_guard<std::mutex> lock(mtx);
	coreId = id;
}

int Process::getCoreId() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return coreId;
}

int Process::getTotalLines() const
{
	return totalInstructions;
}