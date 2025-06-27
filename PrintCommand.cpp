#include "PrintCommand.h"  
#include "Process.h"
#include <chrono>

void PrintCommand::execute(Process& process) {
    // Get the current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    
    // Format the time as a string
    std::string timeStr = std::ctime(&now_c);
    timeStr.pop_back(); // Remove the newline character at the end

    // Create the message with timestamp and PID                                                                   //was process->coreId
    std::string log = "[" + timeStr + "] [PID: " + std::to_string(process.getPID()) + "] " + "Core: " + std::to_string(process.getCoreId()) + " " + msg;
    process.logs.push_back(log);
}