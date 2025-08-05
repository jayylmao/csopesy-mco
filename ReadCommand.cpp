#include "ReadCommand.h"
#include "Process.h"

#include <chrono>
#include <iomanip>
#include <sstream>

// Add Process& if needed (based on your design choice)
ReadCommand::ReadCommand(const std::string& destVar, const std::string& addressStr)
    : ICommand(Type::READ), destVariableName(destVar), addressStr(addressStr) {}

void ReadCommand::execute(Process& process) {
    try {
        // Get memory allocator from process
        auto memoryManager = process.getMemoryAllocator();
        if (!memoryManager) {
            throw std::runtime_error("Memory manager not available");
        }

        // Convert address string to virtual page number
        size_t address;
        if (addressStr.size() > 2 && addressStr.substr(0, 2) == "0x") {
            address = std::stoul(addressStr.substr(2), nullptr, 16);
        }
        else if (std::isdigit(addressStr[0])) {
            address = std::stoul(addressStr);
        }
        else {
            throw std::runtime_error("Invalid address format: " + addressStr);
        }

        // Calculate virtual page number
        size_t pageSize = memoryManager->getPageSize();
        size_t virtualPage = address / pageSize;

        // Access the page (may throw on violation)
        memoryManager->accessPage(process.getPID(), virtualPage);

        // If we get here, memory access was successful
        // Actual read implementation would go here
        uint16_t value = memoryManager->read(process.getPID(), address);
        process.setVar(destVariableName, value);
    }
    catch (const std::exception& e) {
        // Handle memory access violation
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);

        std::ostringstream time_ss;
        time_ss << std::put_time(now_tm, "%H:%M:%S");

        process.setMemoryAccessViolation(time_ss.str(), addressStr);
        process.setFinished(true);  // Terminate the process
    }
}