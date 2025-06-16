#include "PrintCommand.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <fstream>

#include <filesystem> // for checking file existence

/**
 * @brief Instantiate a print command with a specified message.
 */
PrintCommand::PrintCommand(const std::string& msg, int coreId, const std::string& procName, int pid)
    : ICommand(pid, ICommand::PRINT), 
    message(msg), coreId(coreId), procName(procName)
{
}

/**
 * @brief Print a message to a text file.
 */
void PrintCommand::execute() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&currentTime);

    std::ostringstream timeStream;
    timeStream << "("
        << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p")
        << ") Core:" << coreId
        << " \"" << message << "\"";

    // Check if the file already exists
    bool fileExists = std::filesystem::exists(procName + ".txt");

    std::ofstream file(procName + ".txt", std::ios::app);
    if (file.is_open()) {
        if (!fileExists) {
            file << "Process Name: " << procName << "\nLogs:\n" << std::endl;
        }
        file << timeStream.str() << std::endl;
        file.close();
    }
    else {
        std::cerr << "[ERROR] Failed to open file: " << procName << ".txt" << std::endl;
    }
}