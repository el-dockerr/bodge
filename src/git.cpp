#include "git.h"

bool manage_git_repository(const std::string& repo_url, const std::string& local_path) {
    // 1. Check if Git is available
    // std::system returns 0 for success on most systems when the command is found.
    if (std::system(GIT_CHECK_CMD) != 0) {
        std::cerr << "Error: Git is not available. Please install Git and ensure it is in your system's PATH." << std::endl;
        return false;
    }

    // 2. Check if the repository directory exists
    // The 'stat' function (or similar OS-specific functions) is the proper way, 
    // but for simplicity, we'll try to check existence by attempting a pull.
    std::string test_cmd = "test -d " + local_path; // Linux/macOS

    #ifdef _WIN32 // Windows
        // Windows does not have 'test -d', so we use 'if exist'
        std::string win_test_cmd = "if exist " + local_path + " (exit 0) else (exit 1)";
    #endif
    if (std::system(test_cmd.c_str()) == 0) {
        // Directory exists
    } else {
        // Directory does not exist
    }

    // Attempt a pull first. If it succeeds (0), the repo exists.
    std::string pull_cmd = GIT_PULL_CMD(local_path);
    if (std::system(pull_cmd.c_str()) == 0) {
        std::cout << "Repository already cloned. Successfully pulled latest changes." << std::endl;
        return true;
    } else {
        // Pull failed (non-zero return code), likely because the directory doesn't exist
        // or isn't a Git repository. Attempt to clone.
        std::cout << "Repository not found locally. Attempting to clone..." << std::endl;
        
        std::string clone_cmd = GIT_CLONE_CMD(repo_url, local_path);
        if (std::system(clone_cmd.c_str()) == 0) {
            std::cout << "Successfully cloned repository." << std::endl;
            return true;
        } else {
            std::cerr << "Error: Git clone failed. Command: " << clone_cmd << std::endl;
            return false;
        }
    }
}
