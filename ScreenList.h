#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include "ScreenCommands.h"
#include "AConsole.h"

class ScreenList {
public:
	ScreenList(const std::vector<std::unique_ptr<AConsole>>& consoles, int totalCores = 4); //TO-DO: fix totalCores 
    void display() const;

private:
    const std::vector<std::unique_ptr<AConsole>>& consoles;
    int totalCores;

    void printStatusLine(const ScreenS* process, bool isFinished, int coreNumber = -1) const;

    ;
};
