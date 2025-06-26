#pragma once
#include "ICommand.h"
#include <string>

class PrintCommand : public ICommand {
public:
    PrintCommand(std::string msg) : ICommand(Type::PRINT), msg(msg) {}
    PrintCommand(std::string msg, std::string var) : ICommand(Type::PRINT), msg(msg), var(var) {}

    // Override the execute method
    void execute(Process& process) override;
	const std::string& getMsg() const { return msg; }

private:
    std::string msg;
    std::string var;
    uint16_t val;
};