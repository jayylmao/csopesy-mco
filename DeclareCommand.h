#pragma once
#include "ICommand.h"
#include <string>

class DeclareCommand : public ICommand {
public:
    DeclareCommand(const std::string& varName, uint16_t value);

    void execute(Process& process) override;

private:
    std::string variableName;
    uint16_t value;
};
