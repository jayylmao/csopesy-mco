#include "SleepCommand.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

void SleepCommand::execute(Process& process) {
   /*std::cout << "PID: " << process.getPID()
        << " sleeping for " << static_cast<int>(ticks) << " ticks..." << std::endl;*/
    int delay = std::max(1, static_cast<int>(ticks)) * std::max(1, static_cast<int>(ICommand::PER_INSTRUCTION_DELAY_MS));//ensures they dont multiply to 0
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}



