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

	static ConsoleManager* getInstance();

	void initialize();  
	void destroy();

	void addConsole(const std::string& name, std::shared_ptr<AConsole> console);
	std::shared_ptr<AConsole> getConsole(const std::string& name);


	/**
	 * @brief Display the process's info in a console.
	 */
	void drawConsole(Process& process) const;

private:
	ConsoleManager() = default;
	static ConsoleManager* instance;
	ConsoleTable consoleTable;

};