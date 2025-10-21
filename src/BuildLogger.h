#pragma once

#ifndef BUILD_LOGGER_H
#define BUILD_LOGGER_H

#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include "core.h"

/**
 * @brief Class for logging build operations to a file
 */
class BuildLogger {
public:
    /**
     * @brief Constructor
     * @param log_file_path Path to the log file
     */
    explicit BuildLogger(const std::string& log_file_path = "bodge_daemon.log");

    /**
     * @brief Destructor - ensures log file is properly closed
     */
    ~BuildLogger();

    /**
     * @brief Opens the log file for writing
     * @return true if log file was successfully opened
     */
    bool open();

    /**
     * @brief Closes the log file
     */
    void close();

    /**
     * @brief Logs the start of a build
     * @param target_name Name of the target being built (empty for all targets)
     */
    void log_build_start(const std::string& target_name = "");

    /**
     * @brief Logs the completion of a build
     * @param result Build result code
     * @param duration_ms Build duration in milliseconds
     */
    void log_build_end(E_RESULT result, long long duration_ms);

    /**
     * @brief Logs a general message
     * @param message Message to log
     */
    void log_message(const std::string& message);

    /**
     * @brief Logs an error message
     * @param error Error message to log
     */
    void log_error(const std::string& error);

    /**
     * @brief Logs file changes that triggered a build
     * @param changed_files List of changed file paths
     */
    void log_changed_files(const std::vector<std::string>& changed_files);

    /**
     * @brief Checks if the logger is open and ready
     * @return true if logger is ready to write
     */
    bool is_open() const;

private:
    std::string log_file_path_;
    std::ofstream log_file_;
    int build_count_;
    int success_count_;
    int failure_count_;

    /**
     * @brief Gets current timestamp as a formatted string
     * @return Formatted timestamp string
     */
    std::string get_timestamp() const;

    /**
     * @brief Writes a line to the log file with timestamp
     * @param message Message to write
     */
    void write_log_line(const std::string& message);
};

#endif // BUILD_LOGGER_H

