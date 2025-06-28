#include "ICommand.h"
#include <chrono>
#include <thread>

ICommand::ICommand(Type type) : type(type)
{
	// store creation timestamp.
	auto now = std::chrono::system_clock::now();
	std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
	std::tm* time_info = std::localtime(&raw_time);

	std::ostringstream oss;
	oss << std::put_time(time_info, "%m/%d/%Y, %I:%M:%S %p");
	creationTimestamp = oss.str();
}

ICommand::Type ICommand::getType() const
{
    return type;
}

void ICommand::execute(Process& process)
{
    constexpr int PER_INSTRUCTION_DELAY_MS = 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(PER_INSTRUCTION_DELAY_MS));
}

std::string ICommand::getTimestamp()
{
	return creationTimestamp;
}