#pragma once
#include <string>
#include <map>
#include <vector>

class ConfigManager {
public:
    /**
     * @brief Load configuration from a file
     * @param filename Path to configuration file
     */
    void loadConfig(const std::string& filename);

    /**
     * @brief Get the entire configuration map
     * @return Reference to the configuration key-value pairs
     */
    const std::map<std::string, std::string>& getConfig() const;

    /**
     * @brief Print the configuration to console
     */
    void printConfig() const;

private:
    std::map<std::string, std::string> configValues;
};