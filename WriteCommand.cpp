#include "WriteCommand.h"
#include "Process.h"
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>

// Add Process& if needed
WriteCommand::WriteCommand(const std::string& addressStr, const std::string& srcVar)
    : ICommand(Type::WRITE), addressStr(addressStr), sourceVariableName(srcVar) {}

void WriteCommand::execute(Process& process) {
    try {
        // Convert address string to number
        size_t address;
        if (addressStr.size() > 2 && addressStr.substr(0, 2) == "0x") {
            address = std::stoul(addressStr.substr(2), nullptr, 16);
        }
        else {
            address = std::stoul(addressStr);
        }

        // Check bounds
        size_t memoryInBytes = static_cast<size_t>(process.getMemory()) * 1024;
        if (address >= memoryInBytes || (address + sizeof(uint16_t)) > memoryInBytes) {
            throw std::runtime_error("Address out of bounds");
        }

        auto memoryManager = process.getMemoryAllocator();
        if (!memoryManager) {
            throw std::runtime_error("Memory manager not available");
        }

        uint16_t value = process.getVar(sourceVariableName);
        memoryManager->writeUint16(process.getPID(), address, value);
    }
    catch (const std::exception& e) {
        // Handle memory access violation
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);

        std::ostringstream time_ss;
        time_ss << std::put_time(now_tm, "%H:%M:%S");

        process.setMemoryAccessViolation(time_ss.str(), addressStr);
        process.setFinished(true);
    }
}