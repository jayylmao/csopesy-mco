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
	void processSMI();//process SMI

    int getProcessID() const { return processID; }
    std::string getCreationTimestamp() const { return creationTimestamp; }

private:
   // int currentLine;
    //int totalLineCount;
    int processID;
    std::string creationTimestamp;
    ProcessManager* processManager;

    std::string getCurrentTimestamp();
};