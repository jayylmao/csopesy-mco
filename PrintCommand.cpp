#include "PrintCommand.h"
#include "Process.h"
#include <chrono>
#include <iomanip>
#include <sstream>

void PrintCommand::execute(Process& process) {
	// Get the current time
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm local_tm = *std::localtime(&now_c);

	std::ostringstream oss;
	oss << "[" << std::put_time(&local_tm, "%m/%d/%Y %I:%M:%S%p") << "] "
		<< "[PID: " << process.pid << "] "
		<< "Core: " << process.coreId << " "
		<< msg;
	//process.incrementExecutedInstructions();
	process.logs.push_back(oss.str());
}