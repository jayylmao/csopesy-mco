#pragma once
#include "ICommand.h"

class Process;

class SleepCommand : public ICommand {
public:
    SleepCommand(uint8_t ticks)
        : ICommand(Type::SLEEP), ticks(ticks) {
    }

    void execute(Process& process) override;

private:
    uint8_t ticks;
};
