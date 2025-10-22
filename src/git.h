#pragma once

#ifndef GIT_H
#define GIT_H

#include <iostream>
#include <string>
#include <cstdlib>
#include "core.h"

class Git {
public:
    /**
     * @brief Checks for Git availability and clones or pulls a repository.
     * @param repo_url The URL of the Git repository (e.g., "https://github.com/user/repo.git").
     * @param local_path The local directory path for the repository.
     * @return E_RESULT status code indicating success or failure
     */
    E_RESULT manage_git_repository(const std::string& repo_url, const std::string& local_path);

private:
    /**
     * @brief Sanitizes a string for shell command usage
     * @param input The input string to sanitize
     * @return Sanitized string safe for shell commands
     */
    static std::string sanitize_shell_argument(const std::string& input);
    
    /**
     * @brief Validates a git URL
     * @param url The URL to validate
     * @return true if valid, false otherwise
     */
    static bool validate_git_url(const std::string& url);
    
    /**
     * @brief Validates a local path
     * @param path The path to validate
     * @return true if valid, false otherwise
     */
    static bool validate_local_path(const std::string& path);
};
#endif