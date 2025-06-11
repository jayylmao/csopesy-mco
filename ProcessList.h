#pragma once
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
 * @class ProcessList
 * @brief Stores and manages all processes run on the operating system.
 */
class ProcessList {
public:
	ProcessList();

	Process& getProcess();

private:

};