#pragma once
#include <memory>
#include <string>
#include <vector>
#include "AConsole.h"
#include "Process.h"

#include <unordered_map>
#include <Windows.h>

class ConsoleManager {
public:
	// create type definition for table that manages consoles.
	typedef std::unordered_map<std::string, std::shared_ptr<AConsole>> ConsoleTable;

	/**
	 * @brief Display the process's info in a console.
	 */
	void drawConsole(Process& process) const;


	ConsoleTable consoleTable;

};