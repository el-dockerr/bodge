#include "FileWatcher.h"
#include "FileSystemUtils.h"
#include <iostream>
#include <algorithm>

FileWatcher::FileWatcher(const std::vector<std::string>& watch_paths)
    : watch_paths_(watch_paths) {
}

bool FileWatcher::initialize() {
    file_timestamps_.clear();
    
    try {
        for (const std::string& path : watch_paths_) {
            std::filesystem::path fs_path(path);
            
            if (std::filesystem::is_directory(fs_path)) {
                scan_directory(path);
            } else if (std::filesystem::is_regular_file(fs_path)) {
                if (should_watch_file(path)) {
                    file_timestamps_[path] = get_file_time(path);
                }
            } else {
                std::cerr << "[WARNING] Path does not exist or is not accessible: " << path << std::endl;
            }
        }
        
        std::cout << "[INFO] Watching " << file_timestamps_.size() << " file(s) for changes..." << std::endl;
        return !file_timestamps_.empty();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Failed to initialize file watcher: " << e.what() << std::endl;
        return false;
    }
}

bool FileWatcher::has_changes() {
    changed_files_.clear();
    
    try {
        // Check existing files for modifications
        for (auto& [file_path, last_time] : file_timestamps_) {
            if (!std::filesystem::exists(file_path)) {
                // File was deleted
                changed_files_.push_back(file_path);
                continue;
            }
            
            auto current_time = get_file_time(file_path);
            if (current_time != last_time) {
                changed_files_.push_back(file_path);
            }
        }
        
        // Check for new files in watched directories
        for (const std::string& path : watch_paths_) {
            if (std::filesystem::is_directory(path)) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                    if (entry.is_regular_file()) {
                        std::string file_path = entry.path().string();
                        if (should_watch_file(file_path) && 
                            file_timestamps_.find(file_path) == file_timestamps_.end()) {
                            // New file detected
                            changed_files_.push_back(file_path);
                            file_timestamps_[file_path] = get_file_time(file_path);
                        }
                    }
                }
            }
        }
        
        return !changed_files_.empty();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Error checking for file changes: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> FileWatcher::get_changed_files() {
    return changed_files_;
}

void FileWatcher::update_state() {
    try {
        // Update timestamps for all changed files
        for (const std::string& file_path : changed_files_) {
            if (std::filesystem::exists(file_path)) {
                file_timestamps_[file_path] = get_file_time(file_path);
            } else {
                // Remove deleted files from tracking
                file_timestamps_.erase(file_path);
            }
        }
        changed_files_.clear();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Error updating file watcher state: " << e.what() << std::endl;
    }
}

std::vector<std::string> FileWatcher::get_watched_files() const {
    std::vector<std::string> files;
    files.reserve(file_timestamps_.size());
    for (const auto& [file_path, _] : file_timestamps_) {
        files.push_back(file_path);
    }
    return files;
}

void FileWatcher::scan_directory(const std::string& path) {
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                if (should_watch_file(file_path)) {
                    file_timestamps_[file_path] = get_file_time(file_path);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Failed to scan directory " << path << ": " << e.what() << std::endl;
    }
}

bool FileWatcher::should_watch_file(const std::string& path) const {
    return FileSystemUtils::is_cpp_source_file(path) || 
           FileSystemUtils::is_cpp_header_file(path);
}

std::filesystem::file_time_type FileWatcher::get_file_time(const std::string& path) const {
    return std::filesystem::last_write_time(path);
}

