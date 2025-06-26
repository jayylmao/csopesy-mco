#include "ICommand.h"
#include <iostream>

class PrintCommand : public ICommand {
public:
    PrintCommand(int pid) : ICommand(pid, Type::PRINT) {}

    // Override the execute method
    void execute() override;

private:

};