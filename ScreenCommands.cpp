
#include "ScreenCommands.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <thread>
#include <chrono>

ScreenS::ScreenS(const std::string& processName, int totalLines, int pid)
    : AConsole(processName), totalLineCount(totalLines), processID(pid), currentLine(0) {
    creationTimestamp = getCurrentTimestamp();
}

void ScreenS::onEnabled() {
    //onEnabled() is called every time the console first draws to the screen.
    process();
}

void ScreenS::display() {
    std::cout << "\n---------------------------\n";
    std::cout << "Process: " << name << "\n";
    std::cout << "PID: " << processID << "\n\n";
    std::cout << "Current Instruction Line: " << currentLine << "\n";
    std::cout << "Lines of Code: " << totalLineCount << "\n";
    std::cout << "Timestamp of screen creation: " << creationTimestamp << "\n";
    std::cout << "---------------------------\n";
}

void ScreenS::process() {
    std::string input;

    while (true) {
        display();
        std::cout << "\nroot:\> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            std::cout << "[*] Exiting process '" << name << "'...\n";
            break;
        }
        else {
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
