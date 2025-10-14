#pragma once

#include "ProjectConfig.h"
#include "git.h"
#include <string>

/**
 * @brief The main build system class that handles compilation
 */
class BuildSystem {
public:
    /**
     * @brief Constructor that takes the loaded configuration
     * @param config The project configuration to use for building
     */
    explicit BuildSystem(const ProjectConfig& config);

    /**
     * @brief Main function to construct and execute the build command
     * @return true if build was successful, false otherwise
     */
    bool build() const;
    
    /**
     * @brief Builds a specific target
     * @param target_name Name of the target to build
     * @return true if build was successful, false otherwise
     */
    bool build_target(const std::string& target_name) const;
    
    /**
     * @brief Executes a specific sequence
     * @param sequence_name Name of the sequence to execute
     * @return true if sequence was successful, false otherwise
     */
    bool execute_sequence(const std::string& sequence_name) const;

private:
    const ProjectConfig config_;

    /**
     * @brief Constructs the complete compiler command string
     * @return The complete build command as a string
     */
    std::string generate_command() const;

    /**
     * @brief Executes the build command and handles output
     * @param command The command to execute
     * @return true if execution was successful, false otherwise
     */
    bool execute_command(const std::string& command) const;

    /**
     * @brief Validates that the system can execute commands
     * @return true if system command execution is supported
     */
    bool validate_system_support() const;
    
    /**
     * @brief Generates build command for a specific target
     * @param target The target to generate command for
     * @return The complete build command as a string
     */
    std::string generate_target_command(const BuildTarget& target) const;
    
    /**
     * @brief Executes a single operation
     * @param operation The operation to execute
     * @return true if operation was successful, false otherwise
     */
    bool execute_operation(const Operation& operation) const;
    
    /**
     * @brief Copies a file or directory
     * @param source Source path
     * @param destination Destination path
     * @return true if copy was successful, false otherwise
     */
    bool copy_file_or_directory(const std::string& source, const std::string& destination) const;
    
    /**
     * @brief Removes a file or directory
     * @param path Path to remove
     * @return true if removal was successful, false otherwise
     */
    bool remove_file_or_directory(const std::string& path) const;
    
    /**
     * @brief Creates a directory
     * @param path Directory path to create
     * @return true if creation was successful, false otherwise
     */
    bool create_directory(const std::string& path) const;

    /**
     * @brief Builds git dependencies specified in the configuration
     * @return true if all dependencies were successfully handled, false otherwise
     */
    bool build_git_dependencies() const;
};