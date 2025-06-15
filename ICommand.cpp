#include "ICommand.h"
#include <thread>
#include <chrono>

ICommand::ICommand(int pid, Type type) : pid(pid), type(type) {}

ICommand::Type ICommand::getType() const {
    return type;
}

void ICommand::execute() {
    constexpr int PER_INSTRUCTION_DELAY_MS = 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(PER_INSTRUCTION_DELAY_MS));
}