#include "DeclareCommand.h"
#include "Process.h"
#include <iostream>

DeclareCommand::DeclareCommand(const std::string& varName, uint16_t value)
    : ICommand(Type::DECLARE), variableName(varName), value(value) {
}

void DeclareCommand::execute(Process& process) {
    process.setVar(variableName, value);

    //std::string log = "Declared variable '" + variableName + "' = " + std::to_string(value);
    //std::cout << "declare" << std::endl;
}
