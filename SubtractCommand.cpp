#include "SubtractCommand.h"
#include "Process.h"
#include <iostream>
#include <limits>
#include <cctype>

SubtractCommand::SubtractCommand(const std::string& dest, const std::string& lhs, const std::string& rhs)
    : ICommand(Type::SUBTRACT), dest(dest), lhs(lhs), rhs(rhs) {
}

void SubtractCommand::execute(Process& process) {
    uint16_t leftVal = resolveOperand(lhs, process);
    uint16_t rightVal = resolveOperand(rhs, process);

    // Prevent underflow; clamp to 0
    int result = static_cast<int>(leftVal) - static_cast<int>(rightVal);
    uint16_t clampedResult = result < 0 ? 0 : static_cast<uint16_t>(result);

    process.setVar(dest, clampedResult);

    std::string log = "SUB: " + dest + " = " + lhs + " - " + rhs +
        "-> " + std::to_string(leftVal) + " - " + std::to_string(rightVal) +
        " = " + std::to_string(clampedResult);

    std::cout << log << std::endl;
    process.logs.push_back(log);
}

uint16_t SubtractCommand::resolveOperand(const std::string& operand, Process& process) {
    // If it's a number
    if (!operand.empty() && std::isdigit(operand[0])) {
        return static_cast<uint16_t>(std::stoi(operand));
    }

    // If it's a variable
    try {
        return process.getVar(operand);
    }
    catch (...) {
        process.setVar(operand, 0);  // auto-declare
        return 0;
    }
}
