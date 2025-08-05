#include "WriteCommand.h"
#include "Process.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cctype>


// Add Process& if needed
WriteCommand::WriteCommand(const std::string& addressStr, const std::string& srcVar)
    : ICommand(Type::WRITE), addressStr(addressStr), sourceVariableName(srcVar) {}

void WriteCommand::execute(Process& process) {
	try {
		// Get memory allocator
		auto memoryManager = process.getMemoryAllocator();
		if (!memoryManager) {
			throw std::runtime_error("Memory manager not available");
		}

		// Parse address
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

		// Ensure the page is valid (triggers demand paging)
		size_t pageSize = memoryManager->getPageSize();
		size_t virtualPage = address / pageSize;
		memoryManager->accessPage(process.getPID(), virtualPage);

		uint16_t value = process.getVar(sourceVariableName);

		// Write to memory
		memoryManager->write(process.getPID(), address, value);
	}
	catch (const std::exception& e) {
		// Log error and terminate process
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm* now_tm = std::localtime(&now_time);

		std::ostringstream time_ss;
		time_ss << std::put_time(now_tm, "%H:%M:%S");

		process.setMemoryAccessViolation(time_ss.str(), addressStr);
		process.setFinished(true);  // Terminate process
	}
}