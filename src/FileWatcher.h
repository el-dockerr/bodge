#pragma once

#ifndef FILE_WATCHER_H
#define FILE_WATCHER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <filesystem>

/**
 * @brief Class for monitoring file changes in the file system
 */
class FileWatcher {
public:
    /**
     * @brief Constructor
     * @param watch_paths Paths to watch (files or directories)
     */
    explicit FileWatcher(const std::vector<std::string>& watch_paths);

    /**
     * @brief Initializes the file watcher and records initial state
     * @return true if initialization was successful
     */
    bool initialize();

    /**
     * @brief Checks if any watched files have been modified
     * @return true if changes were detected
     */
    bool has_changes();

    /**
     * @brief Gets list of files that have changed since last check
     * @return Vector of changed file paths
     */
    std::vector<std::string> get_changed_files();

    /**
     * @brief Updates the internal state after processing changes
     */
    void update_state();

    /**
     * @brief Gets all files currently being watched
     * @return Vector of all watched file paths
     */
    std::vector<std::string> get_watched_files() const;

private:
    std::vector<std::string> watch_paths_;
    std::map<std::string, std::filesystem::file_time_type> file_timestamps_;
    std::vector<std::string> changed_files_;

    /**
     * @brief Recursively scans directory and adds files to watch list
     * @param path Directory path to scan
     */
    void scan_directory(const std::string& path);

    /**
     * @brief Checks if a file should be watched (C++ source/header files)
     * @param path File path to check
     * @return true if file should be watched
     */
    bool should_watch_file(const std::string& path) const;

    /**
     * @brief Gets the last modification time of a file
     * @param path File path
     * @return File modification time
     */
    std::filesystem::file_time_type get_file_time(const std::string& path) const;
};

#endif // FILE_WATCHER_H

