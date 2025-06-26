#pragma once

#include "ICommand.h"
#include <string>

class SubtractCommand : public ICommand {
public:
    SubtractCommand(const std::string& dest, const std::string& lhs, const std::string& rhs);
    void execute(Process& process) override;

private:
    std::string dest;
    std::string lhs;
    std::string rhs;

    uint16_t resolveOperand(const std::string& operand, Process& process);
};
