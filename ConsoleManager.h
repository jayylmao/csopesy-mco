#pragma once

#include <unordered_map>
#include <memory>
#include "AConsole.h"
#include "Process.h"

class ConsoleManager {
public:
	using ConsoleTable = std::unordered_map<std::string, std::shared_ptr<AConsole>>;

	static ConsoleManager* getInstance();

	void initialize();    
	void destroy();      
	void drawConsole(Process& process) const;  

	void addConsole(const std::string& name, std::shared_ptr<AConsole> console);
	std::shared_ptr<AConsole> getConsole(const std::string& name);

private:
	ConsoleManager() = default;

	ConsoleTable consoleTable;
	static ConsoleManager* instance;
};
