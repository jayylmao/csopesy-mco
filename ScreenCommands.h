#pragma once

#include "AConsole.h"
#include <string>

class ScreenS : public AConsole {
public:
    ScreenS(const std::string& processName, int totalLines, int pid);

    void onEnabled() override;
    void display() override;
    void process() override;

    int getProcessID() const { return processID; }
    int getCurrentLine() const { return currentLine; }
    int getTotalLineCount() const { return totalLineCount; }
    std::string getCreationTimestamp() const { return creationTimestamp; }

private:
    int currentLine;
    int totalLineCount;
    int processID;
    std::string creationTimestamp;

    std::string getCurrentTimestamp();
};