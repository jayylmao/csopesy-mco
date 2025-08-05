#pragma once
#include "ProcessManager.h"
#include "DemandPagingAllocator.h"
#include <string>
#include <memory>

class ProcessManager;

class ScreenS {
public:
    ScreenS(const std::string& processName, int pid, ProcessManager* pm, std::shared_ptr<DemandPagingAllocator> mm);

    void onEnabled();
    void display();
    void process();
	void processSMI();//process SMI

    int getProcessID() const { return processID; }
    std::string getCreationTimestamp() const { return creationTimestamp; }

private:
   // int currentLine;
    //int totalLineCount;
    std::string processName;
    int processID;
    std::string creationTimestamp;
    ProcessManager* processManager;
    std::shared_ptr<DemandPagingAllocator> memoryManager;

    std::string getCurrentTimestamp();
};