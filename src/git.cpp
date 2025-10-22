#include "git.h"
#include <regex>
#include <filesystem>
#include <algorithm>

std::string Git::sanitize_shell_argument(const std::string& input) {
    std::string result;
    result.reserve(input.length() + 2);
    
    // On Windows, use double quotes; on Unix, use single quotes
#ifdef _WIN32
    result += '"';
    for (char c : input) {
        // Escape special characters for Windows
        if (c == '"' || c == '%' || c == '!' || c == '^' || c == '&' || c == '|' || c == '<' || c == '>') {
            result += '^';
        }
        result += c;
    }
    result += '"';
#else
    // For Unix, use single quotes and escape single quotes
    result += '\'';
    for (char c : input) {
        if (c == '\'') {
            result += "'\\''";
        } else {
            result += c;
        }
    }
    result += '\'';
#endif
    
    return result;
}

bool Git::validate_git_url(const std::string& url) {
    if (url.empty() || url.length() > 2048) {
        return false;
    }
    
    // Check for valid git URL patterns
    std::regex git_url_pattern(
        R"(^(https?://|git@|git://|ssh://|file://)[^\s<>\"{}|\\^`\[\]]+$)",
        std::regex_constants::icase
    );
    
    return std::regex_match(url, git_url_pattern);
}

bool Git::validate_local_path(const std::string& path) {
    if (path.empty() || path.length() > 4096) {
        return false;
    }
    
    // Check for path traversal attempts and dangerous characters
    if (path.find("..") != std::string::npos) {
        return false;
    }
    
    // Disallow absolute paths on Unix to prevent writing outside project
#ifndef _WIN32
    if (!path.empty() && path[0] == '/') {
        return false;
    }
#else
    // On Windows, check for absolute paths (drive letters, UNC paths)
    if (path.length() >= 2 && path[1] == ':') {
        return false;
    }
    if (path.length() >= 2 && path[0] == '\\' && path[1] == '\\') {
        return false;
    }
#endif
    
    // Check for dangerous characters
    const std::string dangerous_chars = "<>|\"*?";
    for (char c : dangerous_chars) {
        if (path.find(c) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

E_RESULT Git::manage_git_repository(const std::string& repo_url, const std::string& local_path) {
    // Validate inputs
    if (!validate_git_url(repo_url)) {
        std::cerr << "[ERROR] Invalid git repository URL: " << repo_url << std::endl;
        return S_ERROR_INVALID_ARGUMENT;
    }
    
    if (!validate_local_path(local_path)) {
        std::cerr << "[ERROR] Invalid local path: " << local_path << std::endl;
        return S_ERROR_INVALID_ARGUMENT;
    }
    
    // 1. Check if Git is available
    if (std::system("git --version > /dev/null 2>&1") != 0) {
        std::cerr << "[ERROR] Git is not available. Please install Git and ensure it is in your system's PATH." << std::endl;
        return S_ERROR_RESOURCE_NOT_FOUND;
    }

    // 2. Check if the repository directory exists using std::filesystem
    std::filesystem::path fs_path(local_path);
    bool dir_exists = std::filesystem::exists(fs_path) && std::filesystem::is_directory(fs_path);
    
    if (dir_exists) {
        // Attempt to pull
        std::string pull_cmd = "git -C " + sanitize_shell_argument(local_path) + " pull";
        int result = std::system(pull_cmd.c_str());
        
        if (result == 0) {
            std::cout << "[SUCCESS] Repository already cloned. Successfully pulled latest changes." << std::endl;
            return S_OK;
        } else {
            std::cerr << "[WARNING] Pull failed. Directory exists but may not be a valid git repository." << std::endl;
            return S_GIT_ERROR;
        }
    } else {
        // Directory does not exist, attempt to clone
        std::cout << "[INFO] Repository not found locally. Attempting to clone..." << std::endl;
        
        std::string clone_cmd = "git clone " + sanitize_shell_argument(repo_url) + " " + sanitize_shell_argument(local_path);
        int result = std::system(clone_cmd.c_str());
        
        if (result == 0) {
            std::cout << "[SUCCESS] Successfully cloned repository." << std::endl;
            return S_OK;
        } else {
            std::cerr << "[ERROR] Git clone failed." << std::endl;
            return S_GIT_ERROR;
        }
    }
}
