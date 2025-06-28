#include "AddCommand.h"
#include "Process.h"
#include <iostream>
#include <limits>
#include <cctype>

AddCommand::AddCommand(const std::string& dest, const std::string& lhs, const std::string& rhs)
    : ICommand(Type::ADD), dest(dest), lhs(lhs), rhs(rhs) {
}

void AddCommand::execute(Process& process) {
    uint16_t leftVal = resolveOperand(lhs, process);
    uint16_t rightVal = resolveOperand(rhs, process);

    uint32_t rawSum = static_cast<uint32_t>(leftVal) + rightVal;
    uint16_t clampedSum = (rawSum > std::numeric_limits<uint16_t>::max()) ? 65535 : static_cast<uint16_t>(rawSum);

    process.setVar(dest, clampedSum);

    /*std::string log = "ADD: " + dest + " = " + lhs + " + " + rhs +
        " -> " + std::to_string(leftVal) + " + " + std::to_string(rightVal) +
        " = " + std::to_string(clampedSum);

    std::cout << "add" << std::endl;*/
}

uint16_t AddCommand::resolveOperand(const std::string& operand, Process& process) {
    // Try parsing a literal value
    if (!operand.empty() && std::isdigit(operand[0])) {
        return static_cast<uint16_t>(std::stoi(operand));
    }

    // Otherwise, treat it as a variable. Default to 0 if undeclared.
    try {
        return process.getVar(operand);
    }
    catch (...) {
        process.setVar(operand, 0); // Auto-declare
        return 0;
    }
}