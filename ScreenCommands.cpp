#include "Process.h"
#include "ScreenCommands.h"
#include "ProcessManager.h"
#include "IMemoryAllocator.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <thread>
#include <chrono>


std::mutex screenMutex;

ScreenS::ScreenS(const std::string& processName, int pid, ProcessManager* pm, std::shared_ptr<IMemoryAllocator> mm)
    : processName(processName), processID(pid), processManager(pm), memoryManager(mm) {
    creationTimestamp = getCurrentTimestamp();
}

//onEnabled() is called every time the console first draws to the screen.
void ScreenS::onEnabled() {
    process();
}

void ScreenS::display() {
    int currentLine = 0;
    int totalLines = 0;

    if (processManager) {
        try {
            std::shared_ptr<Process> process = processManager->getSharedProcess(processName);
            currentLine = process->getCurrentLine();
            totalLines = process->getTotalLines();
        }
        catch (...) {
            std::cout << "[!] Could not fetch process state for PID " << processID << "\n";
        }

        std::cout << "\n---------------------------\n";
        std::cout << "Process: " << processName << "\n";
        std::cout << "PID: " << processID << "\n\n";
        std::cout << "Current Instruction Line: " << currentLine << "\n";
        std::cout << "Lines of Code: " << totalLines << "\n";
        std::cout << "Timestamp of screen creation: " << creationTimestamp << "\n";
        std::cout << "---------------------------\n";
        std::cout << memoryManager->displayMemory();
    }
}
//NEW ONE
void ScreenS::processSMI() {
    
        std::lock_guard<std::mutex> lock(screenMutex);
        if (processManager) {
            try {
                std::shared_ptr<Process> process = processManager->getSharedProcess(processName);
                std::cout << "\nProcess Name: " << process->getName() << "\n";
                std::cout << "PID: " << process->getPID() << "\n";
                std::cout << "Logs:\n";
                for (const auto& log : process->logs) {
                    std::cout << log << "\n";
                }
                std::cout << "\nCurrent instruction line: " << process->getCurrentLine() << "\n";
                std::cout << "Lines of code: " << process->getTotalLines() << "\n";
                if (process->hasFinished()) {
                    std::cout << "\nFinished!\n";
                }
            }
            catch (...) {
                std::cout << "[!] Could not fetch process state.\n";
            }
        }
    
	std::cout << "---------------------\n";
}
//TILL HERE

void ScreenS::process() {
    std::string input;
	bool skipDisplay = false;

    while (true) {
        if(!skipDisplay)
            display();
        skipDisplay =false;

        std::cout << "\n type exit to Exit. \n";
        std::cout << "\nroot:\\> ";
        std::getline(std::cin, input);

        if (input == "exit") 
        {
            std::cout << "[*] Exiting process '" << processName << "'...\n";
            break;
        }
        else if(input == "process-smi") 
        {
			processSMI();
            skipDisplay= true;
        }
        else 
        {
            continue; 
        }
    }
}

std::string ScreenS::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
    std::tm* time_info = std::localtime(&raw_time);

    std::ostringstream oss;
    oss << std::put_time(time_info, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}

