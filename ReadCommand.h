#pragma once
#include "ICommand.h"
#include <string>

/**
 * @class ReadCommand
 * @brief Reads a value from memory into a variable.
 *
 * Usage: READ <var_name> <memory_address>
 * Example: READ result 0x500
 */
class ReadCommand : public ICommand {
public:
    ReadCommand(const std::string& destVarName, const std::string& memoryAddress); //changelater

    void execute(Process& process) override;

private:
    std::string destVariableName;  // Variable to store the read value
    std::string addressStr;        // Memory address (as string to support hex/dec)
};