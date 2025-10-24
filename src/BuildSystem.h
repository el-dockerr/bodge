#pragma once

#ifndef BUILD_SYSTEM_H
#define BUILD_SYSTEM_H

#include "ProjectConfig.h"
#include "git.h"
#include "core.h"
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
     * @return S_OK if build was successful, S_BUILD_FAILED otherwise
     */
    E_RESULT build() const;

    /**
     * @brief Builds git dependencies specified in the configuration
     * @return S_OK if all dependencies were successfully handled, S_BUILD_FAILED otherwise
     */
    E_RESULT build_git_dependencies_only() const;

    /**
     * @brief Builds a specific target
     * @param target_name Name of the target to build
     * @return S_OK if build was successful, S_BUILD_FAILED otherwise
     */
    E_RESULT build_target(const std::string& target_name) const;
    
    /**
     * @brief Builds a specific target for a specific platform
     * @param target_name Name of the target to build
     * @param platform Target platform to build for
     * @return S_OK if build was successful, S_BUILD_FAILED otherwise
     */
    E_RESULT build_target_for_platform(const std::string& target_name, const Platform& platform) const;

    /**
     * @brief Executes a specific sequence
     * @param sequence_name Name of the sequence to execute
     * @return S_OK if sequence was successful, S_COMMAND_EXECUTION_FAILED otherwise
     */
    E_RESULT execute_sequence(const std::string& sequence_name) const;

    /**
     * @brief Runs the build system in daemon mode (watches for file changes)
     * @param poll_interval_ms Interval in milliseconds between file checks (default: 1000ms)
     * @param log_file Path to log file (default: "bodge_daemon.log")
     * @return S_OK if daemon started successfully (runs indefinitely until interrupted)
     */
    E_RESULT run_daemon_mode(int poll_interval_ms = 1000, const std::string& log_file = "bodge_daemon.log") const;

private:
    const ProjectConfig config_;


    /**
     * @brief Performs pre-build checks
     * @return S_OK if checks were successful, S_ERROR_INVALID_ARGUMENT otherwise
     */
    E_RESULT prebuild_checks() const;

    /**
     * @brief Constructs the complete compiler command string
     * @return The complete build command as a string
     */
    std::string generate_command() const;

    /**
     * @brief Executes the build command and handles output
     * @param command The command to execute
     * @return S_OK if execution was successful, S_COMMAND_EXECUTION_FAILED otherwise
     */
    E_RESULT execute_command(const std::string& command) const;

    /**
     * @brief Validates that the system can execute commands
     * @return S_OK if system command execution is supported, S_FAILURE otherwise
     */
    E_RESULT validate_system_support() const;
    
    /**
     * @brief Generates build command for a specific target
     * @param target The target to generate command for
     * @return The complete build command as a string
     */
    std::string generate_target_command(const BuildTarget& target) const;
    
    /**
     * @brief Generates build command for a specific target and platform
     * @param target The target to generate command for
     * @param platform The platform to build for
     * @return The complete build command as a string
     */
    std::string generate_target_command_for_platform(const BuildTarget& target, const Platform& platform) const;
    
    /**
     * @brief Gets the platforms to build for (either from config or current platform)
     * @return Vector of platforms to build for
     */
    std::vector<Platform> get_target_platforms() const;
    
    /**
     * @brief Executes a single operation
     * @param operation The operation to execute
     * @return S_OK if operation was successful, S_FAILURE otherwise
     */
    E_RESULT execute_operation(const Operation& operation) const;
    
    /**
     * @brief Copies a file or directory
     * @param source Source path
     * @param destination Destination path
     * @return S_OK if copy was successful, S_FILE_OPERATION_FAILED otherwise
     */
    E_RESULT copy_file_or_directory(const std::string& source, const std::string& destination) const;
    
    /**
     * @brief Removes a file or directory
     * @param path Path to remove
     * @return S_OK if removal was successful, S_FILE_OPERATION_FAILED otherwise
     */
    E_RESULT remove_file_or_directory(const std::string& path) const;
    
    /**
     * @brief Creates a directory
     * @param path Directory path to create
     * @return S_OK if creation was successful, S_FILE_OPERATION_FAILED otherwise
     */
    E_RESULT create_directory(const std::string& path) const;

    /**
     * @brief Builds git dependencies specified in the configuration
     * @return S_OK if all dependencies were successfully handled, S_BUILD_FAILED otherwise
     */
    E_RESULT build_git_dependencies() const;
};

#endif // BUILD_SYSTEM_H