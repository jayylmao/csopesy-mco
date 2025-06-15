#include "PrintCommand.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <ofstream>

/**
 * @brief Instantiate a print command with a specified message.
 */
PrintCommand::PrintCommand(std::string& msg)
{
	this->message = msg;
}

/**
 * @brief Print a message to a text file.
 */
PrintCommand::execute()
{
	//std::ofstream file;
	//std::string procName;
	//file.open(procName + ".txt");

	std::string procName = getCurrentProcessName();
	int coreId = getCurrentCoreID();

	// Get current time
	auto now = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
	std::tm localTime = *std::localtime(&currentTime);

	// Format timestamp: (MM/DD/YYYY HH:MM:SSAM/PM)
	std::ostringstream timeStream;
	timeStream << "("
		<< std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p")
		<< ") Core:" << coreId
		<< " \"" << message << "\"";

	// Open the process's log file in append mode
	std::ofstream file(procName + ".txt", std::ios::app);
	if (file.is_open()) {
		file << timeStream.str() << std::endl;
		file.close();
	}
}