#pragma once
#include "ICommand.h"
#include <string>

/**
 * @class WriteCommand
 * @brief Writes a variable's value to memory.
 *
 * Usage: WRITE <memory_address> <var_name>
 * Example: WRITE 0x500 sourceVar
 */
class WriteCommand : public ICommand {
public:
    WriteCommand(const std::string& memoryAddress, const std::string& sourceVarName); //change later

    void execute(Process& process) override;

private:
    std::string addressStr;        // Memory address (as string to support hex/dec)
    std::string sourceVariableName; // Variable whose value will be written
};