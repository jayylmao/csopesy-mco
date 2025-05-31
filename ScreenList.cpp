#include "ScreenList.h"
#include "ScreenCommands.h"
#include <iostream>
#include <algorithm>

ScreenList::ScreenList(const std::vector<std::unique_ptr<AConsole>>& consoles, int totalCores)
    : consoles(consoles), totalCores(totalCores) {
}

void ScreenList::display() const {
    std::vector<const ScreenS*> runningProcesses;
    std::vector<const ScreenS*> finishedProcesses;

    for (const auto& console : consoles) {
        const ScreenS* screenProcess = dynamic_cast<ScreenS*>(console.get());
        if (screenProcess) {
            if (screenProcess->getCurrentLine() < screenProcess->getTotalLineCount()) {
                runningProcesses.push_back(screenProcess);
            }
            else {
                finishedProcesses.push_back(screenProcess);
            }
        }
    }

    // CPU usage summary
    int runningCount = runningProcesses.size();
    int finishedCount = finishedProcesses.size();
    int usedCores = runningCount;
    int availableCores = std::max(0, totalCores - usedCores);
    int cpuUtil = (usedCores * 100) / totalCores;

    std::cout << "\nCPU Utilization: " << cpuUtil << "%\n";
    std::cout << "Cores Used: " << usedCores << "\n";
    std::cout << "Cores Available: " << availableCores << "\n";

    std::cout << "\nRunning Processes (" << runningCount << "):\n";
    for (size_t i = 0; i < runningProcesses.size(); ++i) {
        int coreNumber = (i < totalCores) ? i : -1;
        printStatusLine(runningProcesses[i], false);
    }

    std::cout << "\nFinished Processes (" << finishedCount << "):\n";
    for (const ScreenS* process : finishedProcesses) {
        printStatusLine(process, true);
    }
}


void ScreenList::printStatusLine(const ScreenS* process, bool isFinished) const {
    std::cout << process->getName() << "\t(" << process->getCreationTimestamp() << ")\t";
        
    if (isFinished) {
        std::cout << "Finished\t" << process->getTotalLineCount() << " / " << process->getTotalLineCount() << "\n";
    }
    else {
        std::cout << "Core: # \t\t" << process->getCurrentLine() << " / " << process->getTotalLineCount() << "\n";
    }
}