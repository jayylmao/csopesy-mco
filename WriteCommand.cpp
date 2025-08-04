#include "WriteCommand.h"
#include "Process.h"

// Add Process& if needed
WriteCommand::WriteCommand(const std::string& addressStr, const std::string& srcVar)
    : ICommand(Type::WRITE), addressStr(addressStr), sourceVariableName(srcVar) {}

void WriteCommand::execute(Process& process) {
    // Implementation here
}