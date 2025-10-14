#pragma once

#ifndef GIT_H
#define GIT_H

#include <iostream>
#include <string>
#include <cstdlib> // Required for std::system

// Define the Git commands
#define GIT_CHECK_CMD "git --version"
#define GIT_CLONE_CMD(repo, path) ("git clone " + repo + " " + path)
#define GIT_PULL_CMD(path) ("git -C " + path + " pull")


class Git {
    public:
    /**
     * @brief Checks for Git availability and clones or pulls a repository.
     * @param repo_url The URL of the Git repository (e.g., "https://github.com/user/repo.git").
     * @param local_path The local directory path for the repository.
     * @return true if the operation (clone or pull) was successful, false otherwise.
     */
    bool manage_git_repository(const std::string& repo_url, const std::string& local_path);
};
#endif