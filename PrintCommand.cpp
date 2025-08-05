#include "PrintCommand.h"
#include "Process.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

PrintCommand::PrintCommand(const std::string& msg)
    : ICommand(Type::PRINT), printElements({ msg }) {
}
PrintCommand::PrintCommand(const std::vector<std::string>& elements)
    : ICommand(Type::PRINT), printElements(elements) {}

void PrintCommand::execute(Process& process) {
    std::string output;  

    //std::cout << "Executing with elements: ";
    for (const auto& e : printElements) std::cout << "'" << e << "' ";
    std::cout << "\n";

    for (const auto& element : printElements) {
        if (element.empty()) continue;

        //std::cout << "Processing element: '" << element << "'\n";

        // Handle quoted strings
        if (element.front() == '"' && element.back() == '"') {
            output += element.substr(1, element.size() - 2);
        }
        // Handle variables
        else {
            try {
                output += std::to_string(process.getVar(element));
            }
            catch (...) {
                output += "[UNKNOWN_VAR]";
            }
        }
    }




	// Get the current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_c);

    std::ostringstream oss;
    oss << "[" << std::put_time(&local_tm, "%m/%d/%Y %I:%M:%S%p") << "] "
        << "[PID: " << process.getPID() << "] "
        << "Core: " << process.getCoreId() << " "
        << output;

    process.logs.push_back(oss.str());
}