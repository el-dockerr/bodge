#pragma once

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "ProjectConfig.h"
#include <string>

/**
 * @brief Handles parsing of configuration files for the build system
 */
class ConfigParser {
public:
    /**
     * @brief Loads project configuration from the specified file
     * @param filename The path to the configuration file (e.g., ".bodge")
     * @return A fully populated ProjectConfig struct
     */
    static ProjectConfig load_project_config(const std::string& filename);

private:
    /**
     * @brief Processes a single line from the configuration file
     * @param line The line to process
     * @param config Reference to the configuration object to populate
     */
    static void process_config_line(const std::string& line, ProjectConfig& config);
    
    /**
     * @brief Processes target-specific configuration lines
     * @param key The configuration key (e.g., "mylib.sources")
     * @param value The configuration value
     * @param config Reference to the configuration object to populate
     */
    static void process_target_config_line(const std::string& key, const std::string& value, ProjectConfig& config);
    
    /**
     * @brief Processes sequence configuration lines
     * @param key The configuration key (e.g., "sequence.build_all")
     * @param value The configuration value
     * @param config Reference to the configuration object to populate
     */
    static void process_sequence_config_line(const std::string& key, const std::string& value, ProjectConfig& config);
    
    /**
     * @brief Parses build type from string
     * @param type_str String representation of build type
     * @return BuildType enum value
     */
    static BuildType parse_build_type(const std::string& type_str);
};

#endif // CONFIG_PARSER_H