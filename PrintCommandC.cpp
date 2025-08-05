#include "PrintCommandC.h"
#include "Process.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>



PrintCommandC::PrintCommandC(const std::vector<std::string>& elements, std::vector<std::string>& logs)
    : ICommand(Type::PRINT), printElements(elements), logsToEdit(logs) {
}

void PrintCommandC::execute(Process& process) {
    std::string output;

    for (const auto& element : printElements) {
        if (element.empty()) continue;

        // Handle quoted strings
        if (element.size() >= 4 &&
            element.substr(0, 2) == "\\\"" &&
            element.substr(element.size() - 2) == "\\\"") {

            // Verify no unescaped quotes inside
            bool valid = true;
            for (size_t i = 2; i < element.size() - 2; ++i) {
                if (element[i] == '"' && (i == 0 || element[i - 1] != '\\')) {
                    valid = false;
                    break;
                }
            }

            if (!valid) {
                output += "[MALFORMED_STRING]";
                continue;
            }

            // Extract and unescape content
            std::string content = element.substr(2, element.size() - 4);
            std::string unescaped;
            for (size_t i = 0; i < content.size(); ++i) {
                if (content[i] == '\\' && i + 1 < content.size() && content[i + 1] == '"') {
                    unescaped += '"';
                    ++i;
                }
                else {
                    unescaped += content[i];
                }
            }
            output += unescaped;
        }
        // Handle variables
        else {
            // Validate variable name
            if (element.find_first_of(" \t\"") != std::string::npos) {
                output += "[INVALID_VAR_NAME]";
                continue;
            }
            try {
                output += std::to_string(process.getVar(element));
            }
            catch (...) {
                output += "[UNKNOWN_VAR]";
            }
        }
    }

    // Log the output
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