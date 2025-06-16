#include "ConsoleManager.h"

ConsoleManager* ConsoleManager::instance = nullptr;

ConsoleManager* ConsoleManager::getInstance() {
    if (!instance) {
        instance = new ConsoleManager();
    }
    return instance;
}

void ConsoleManager::initialize() {
    // Optional startup logic
}

void ConsoleManager::destroy() {
    // Optional cleanup
}

void ConsoleManager::addConsole(const std::string& name, std::shared_ptr<AConsole> console) {
    consoleTable[name] = console;
}

std::shared_ptr<AConsole> ConsoleManager::getConsole(const std::string& name) {
    auto it = consoleTable.find(name);
    if (it != consoleTable.end()) {
        return it->second;
    }
    return nullptr;
}

void ConsoleManager::drawConsole(Process& process) const {
    auto it = consoleTable.find(process.getName());
    if (it != consoleTable.end()) {
        it->second->display();
    }
}