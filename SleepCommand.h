#pragma once
#include "ICommand.h"
#include "Process.h"
#include <iostream>
#include <thread>
#include <chrono>

class SleepCommand : public ICommand {
public:
    SleepCommand(uint8_t ticks)
        : ICommand(Type::SLEEP), ticks(ticks) {
    }

    void execute(Process& process) override;

private:
    uint8_t ticks;
};
