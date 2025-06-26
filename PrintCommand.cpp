#include "PrintCommand.h"  
#include "Process.h"
#include <chrono>

void PrintCommand::execute(Process& process) {
	
	//std::cout << "[PID: " << process.getPID() << "] " << msg << std::endl; 
    process.logs.push_back(msg);
}