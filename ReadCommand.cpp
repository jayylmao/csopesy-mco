#include "ReadCommand.h"
#include "Process.h"

// Add Process& if needed (based on your design choice)
ReadCommand::ReadCommand(const std::string& destVar, const std::string& addressStr)
    : ICommand(Type::READ), destVariableName(destVar), addressStr(addressStr) {}

void ReadCommand::execute(Process& process) {
    // Implementation here
}