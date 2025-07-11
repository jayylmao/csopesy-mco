#include "ConfigManager.h"
#include <fstream>
#include <iostream>
#include <cctype> // for isspace

#ifdef _WIN32
#include <direct.h> // for _getcwd
#else
#include <unistd.h> // for getcwd
#endif

void ConfigManager::loadConfig(const std::string& filename) {
    // Get current working directory
    char buffer[FILENAME_MAX];
#ifdef _WIN32
    if (_getcwd(buffer, FILENAME_MAX)) {
#else
    if (getcwd(buffer, FILENAME_MAX)) {
#endif
        std::cout << "[i] Current working directory: " << buffer << std::endl;
    }
    else {
        std::cerr << "[!] Could not get working directory" << std::endl;
    }

    // Try to open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[!] Could not open config file: " << filename << std::endl;
        std::cerr << "[!] Make sure config.txt is in the working directory: " << buffer << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Trim leading whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue; // Skip empty lines
        line = line.substr(start);

        // Skip comments
        if (line[0] == '#') continue;

        // Find the position to split key-value
        size_t split_pos = line.find(' ');
        if (split_pos == std::string::npos) {
            configValues[line] = "";
            continue;
        }

        std::string key = line.substr(0, split_pos);

        // Find start of value (skip whitespace)
        size_t value_start = line.find_first_not_of(" \t", split_pos + 1);
        if (value_start == std::string::npos) {
            configValues[key] = "";
            continue;
        }

        std::string value = line.substr(value_start);

        // Remove quotes if present
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        configValues[key] = value;
    }
}

const std::map<std::string, std::string>& ConfigManager::getConfig() const {
    return configValues;
}

void ConfigManager::printConfig() const {
    std::cout << "Configuration values:\n";
    std::cout << "----------------------\n";
    for (const auto& pair : configValues) {
        std::cout << pair.first << " = " << pair.second << "\n";
    }
    std::cout << "----------------------\n";
}