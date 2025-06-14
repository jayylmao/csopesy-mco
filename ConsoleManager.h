#pragma once
#include <memory>
#include <vector>
#include "AConsole.h"

#include <unordered_map>
#include <Windows.h>

class ConsoleManager {
public:
	// create type definition for table that manages consoles.
	typedef std::unordered_map<String, std::shared_ptr<AConsole>> ConsoleTable;

	/**
	 * @brief Get instance of Console Manager.
	 * @return Instance of Console Manager.
	 */
	static ConsoleManager* getInstance();

	/**
	 * @brief Initialize console manager.
	 */
	static void initialize();

	/**
	 * @brief Destroy instance when finished.
	 */
	static void destroy();

	/**
	 * @brief Print the 
	 */
	void drawConsole() const;

private:
	ConsoleManager();
};