#pragma once

#include "AConsole.h"
#include <string>

class ScreenS : public AConsole {
public:
    ScreenS(const std::string& processName, int totalLines, int pid);

    void onEnabled() override;
    void display() override;
    void process() override;

private:
    int currentLine;
    int totalLineCount;
    int processID;
    std::string creationTimestamp;

    std::string getCurrentTimestamp();
};