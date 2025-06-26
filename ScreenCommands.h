#pragma once
#include "ProcessManager.h"
#include "AConsole.h"
#include <string>

class ProcessManager;

class ScreenS : public AConsole {
public:
    ScreenS(const std::string& processName, int pid, ProcessManager* pm);

    void onEnabled() override;
    void display() override;
    void process() override;

    int getProcessID() const { return processID; }
   // int getCurrentLine() const { return currentLine; } was for simulation
    //int getTotalLineCount() const { return totalLineCount; }
    std::string getCreationTimestamp() const { return creationTimestamp; }

private:
   // int currentLine;
    //int totalLineCount;
    int processID;
    std::string creationTimestamp;
    ProcessManager* processManager;

    std::string getCurrentTimestamp();
};