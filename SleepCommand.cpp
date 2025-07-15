#include "SleepCommand.h"

void SleepCommand::execute(Process& process) {
   /*std::cout << "PID: " << process.getPID()
        << " sleeping for " << static_cast<int>(ticks) << " ticks..." << std::endl;*/
   // process.incrementExecutedInstructions();
    std::this_thread::sleep_for(std::chrono::milliseconds(ticks * 100));
}



