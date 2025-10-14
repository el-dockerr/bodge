#include "BuildSystem.h"
#include "StringUtils.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

BuildSystem::BuildSystem(const ProjectConfig& config) : config_(config) {}

bool BuildSystem::build() const {
    std::cout << "--- Bodge: The Idiotic Build System ---" << std::endl;

    // Validate system support
    if (!validate_system_support()) {
        std::cerr << "System command execution is not fully supported on this platform. "
                  << "Build may fail." << std::endl;
    }

    // Check for essential inputs
    if (!config_.is_valid()) {
        std::cerr << "[ERROR] Configuration is invalid. Please check your .bodge file." << std::endl;
        return false;
    }

    // Handle git dependencies first
    if (!build_git_dependencies()) {
        std::cerr << "[ERROR] Failed to handle git dependencies." << std::endl;
        return false;
    }

    // If we have targets, build all of them
    if (!config_.targets.empty()) {
        bool all_success = true;
        for (const auto& [name, target] : config_.targets) {
            if (!build_target(name)) {
                all_success = false;
            }
        }
        return all_success;
    }
    
    // Legacy single build
    std::string command = generate_command();
    return execute_command(command);
}

bool BuildSystem::build_git_dependencies_only() const {
    return build_git_dependencies();
}

std::string BuildSystem::generate_command() const {
    std::stringstream command;

    // 1. Compiler
    command << config_.compiler;

    // 2. Standard CXX Flags
    command << " " << StringUtils::join(config_.cxx_flags, "", " ");

    // 3. Include Directories (-I)
    command << StringUtils::join(config_.include_dirs, " -I", " ");

    // 4. Source Files (must come before -o and linking flags)
    command << StringUtils::join(config_.sources, "", " ");

    // 5. Output file (-o)
    command << " -o " << config_.output_name;

    // 6. Library Directories (-L)
    command << StringUtils::join(config_.library_dirs, " -L", " ");

    // 7. Libraries (-l)
    command << StringUtils::join(config_.libraries, " -l", " ");

    return command.str();
}

bool BuildSystem::execute_command(const std::string& command) const {
    std::cout << "\n[INFO] Executing command (The Idiot Way):\n" << command << "\n" << std::endl;

    // Execute the command using the system shell
    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << "\n[SUCCESS] Project '" << config_.output_name << "' built successfully." << std::endl;
        return true;
    } else {
        std::cerr << "\n[ERROR] Bodge encountered an issue (Exit Code: " << result 
                  << "). Check the compiler output for details." << std::endl;
        return false;
    }
}

bool BuildSystem::validate_system_support() const {
    return std::system(nullptr) != 0;
}

bool BuildSystem::build_target(const std::string& target_name) const {
    auto it = config_.targets.find(target_name);
    if (it == config_.targets.end()) {
        std::cerr << "[ERROR] Target '" << target_name << "' not found." << std::endl;
        return false;
    }
    
    const BuildTarget& target = it->second;
    if (!target.is_valid()) {
        std::cerr << "[ERROR] Target '" << target_name << "' is invalid." << std::endl;
        return false;
    }
    
    std::cout << "\n[INFO] Building target: " << target_name << std::endl;
    std::string command = generate_target_command(target);
    return execute_command(command);
}

bool BuildSystem::execute_sequence(const std::string& sequence_name) const {
    auto it = config_.sequences.find(sequence_name);
    if (it == config_.sequences.end()) {
        std::cerr << "[ERROR] Sequence '" << sequence_name << "' not found." << std::endl;
        return false;
    }
    
    const Sequence& sequence = it->second;
    std::cout << "\n[INFO] Executing sequence: " << sequence_name << std::endl;
    
    for (const Operation& op : sequence.operations) {
        if (!execute_operation(op)) {
            std::cerr << "[ERROR] Sequence '" << sequence_name << "' failed." << std::endl;
            return false;
        }
    }
    
    std::cout << "[SUCCESS] Sequence '" << sequence_name << "' completed successfully." << std::endl;
    return true;
}

std::string BuildSystem::generate_target_command(const BuildTarget& target) const {
    std::stringstream command;
    
    // 1. Compiler
    command << config_.compiler;
    
    // 2. Global CXX Flags
    command << " " << StringUtils::join(config_.global_cxx_flags, "", " ");
    
    // 3. Target-specific CXX Flags
    command << " " << StringUtils::join(target.cxx_flags, "", " ");
    
    // 4. Build type specific flags
    switch (target.type) {
        case BuildType::SHARED_LIBRARY:
#ifdef _WIN32
            command << " -shared";
#else
            command << " -shared -fPIC";
#endif
            break;
        case BuildType::STATIC_LIBRARY:
            // Note: Static libraries need different handling (ar command)
            // For now, we'll treat them as objects that need to be archived
            break;
        case BuildType::EXECUTABLE:
            // No special flags needed
            break;
    }
    
    // 5. Global Include Directories (-I)
    command << StringUtils::join(config_.global_include_dirs, " -I", " ");
    
    // 6. Target-specific Include Directories (-I)
    command << StringUtils::join(target.include_dirs, " -I", " ");
    
    // 7. Source Files
    command << StringUtils::join(target.sources, "", " ");
    
    // 8. Output file (-o)
    std::string output_name = target.output_name + target.get_output_extension();
    command << " -o " << output_name;
    
    // 9. Global Library Directories (-L)
    command << StringUtils::join(config_.global_library_dirs, " -L", " ");
    
    // 10. Target-specific Library Directories (-L)
    command << StringUtils::join(target.library_dirs, " -L", " ");
    
    // 11. Global Libraries (-l)
    command << StringUtils::join(config_.global_libraries, " -l", " ");
    
    // 12. Target-specific Libraries (-l)
    command << StringUtils::join(target.libraries, " -l", " ");
    
    return command.str();
}

bool BuildSystem::build_git_dependencies() const {
    if (config_.dependencies_url.empty() && config_.dependencies_path.empty()) {
        return true; // No dependencies to handle
    }
    if (config_.dependencies_url.size() != config_.dependencies_path.size()) {
        std::cerr << "[ERROR] Mismatch between number of dependency git URLs and git paths." << std::endl;
        return false;
    }

    Git git;
    int i = 0;
    
    for (const auto& url : config_.dependencies_url) {
        std::cout << "[INFO] Cloning dependency from " << url << std::endl;
        try {
            E_RESULT res = git.manage_git_repository(url, config_.dependencies_path[i]);
            if (res != S_OK) {
                std::cerr << "[ERROR] Failed to clone " << url << std::endl;
                return false;
            } else if (res == S_OK) {
                std::cout << "[SUCCESS] Cloned " << url << std::endl;
                if (!config_.run_bodge_after_clone.empty()) {
                    std::cout << "[INFO] Running post-clone command: " << config_.run_bodge_after_clone << std::endl;
                    if (config_.run_bodge_after_clone == "true") {
                        // Save current directory
                        std::filesystem::path original_path = std::filesystem::current_path();
                        
                        try {
                            // Change to the cloned repository directory
                            std::filesystem::current_path(config_.dependencies_path[i]);
                            
                            // Run bodge in the repository directory
                            std::cout << "[INFO] Running bodge in " << config_.dependencies_path[i] << std::endl;
                            if (std::system("bodge") != 0) {
                                std::cerr << "[ERROR] Post-clone bodge command failed." << std::endl;
                                // Restore original directory before returning
                                std::filesystem::current_path(original_path);
                                return false;
                            }
                            
                            // Restore original directory
                            std::filesystem::current_path(original_path);
                            std::cout << "[SUCCESS] Post-clone bodge command completed." << std::endl;
                        } catch (const std::filesystem::filesystem_error& e) {
                            std::cerr << "[ERROR] Failed to change directory: " << e.what() << std::endl;
                            // Attempt to restore original directory
                            try {
                                std::filesystem::current_path(original_path);
                            } catch (...) {}
                            return false;
                        }
                    }
                }
                i++;
            } else {
                std::cerr << "[ERROR] Failed to clone " << url << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception during git operation: " << e.what() << std::endl;
            return false;
        }
    }
    return true;
}

bool BuildSystem::execute_operation(const Operation& operation) const {
    switch (operation.type) {
        case OperationType::BUILD:
            return build_target(operation.target);
        case OperationType::COPY:
            return copy_file_or_directory(operation.target, operation.destination);
        case OperationType::REMOVE:
            return remove_file_or_directory(operation.target);
        case OperationType::MKDIR:
            return create_directory(operation.target);
    }
    return false;
}

bool BuildSystem::copy_file_or_directory(const std::string& source, const std::string& destination) const {
    std::cout << "[INFO] Copying " << source << " -> " << destination << std::endl;
    
    try {
        std::filesystem::path src_path(source);
        std::filesystem::path dst_path(destination);
        
        if (std::filesystem::is_directory(src_path)) {
            std::filesystem::copy(src_path, dst_path, 
                std::filesystem::copy_options::recursive | 
                std::filesystem::copy_options::overwrite_existing);
        } else {
            // Create parent directory if it doesn't exist
            std::filesystem::create_directories(dst_path.parent_path());
            std::filesystem::copy_file(src_path, dst_path, 
                std::filesystem::copy_options::overwrite_existing);
        }
        
        std::cout << "[SUCCESS] Copy completed." << std::endl;
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Copy failed: " << e.what() << std::endl;
        return false;
    }
}

bool BuildSystem::remove_file_or_directory(const std::string& path) const {
    std::cout << "[INFO] Removing " << path << std::endl;
    
    try {
        std::filesystem::path fs_path(path);
        std::uintmax_t removed_count = std::filesystem::remove_all(fs_path);
        
        if (removed_count > 0) {
            std::cout << "[SUCCESS] Removed " << removed_count << " item(s)." << std::endl;
        } else {
            std::cout << "[INFO] Nothing to remove (path doesn't exist)." << std::endl;
        }
        
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Remove failed: " << e.what() << std::endl;
        return false;
    }
}

bool BuildSystem::create_directory(const std::string& path) const {
    std::cout << "[INFO] Creating directory " << path << std::endl;
    
    try {
        std::filesystem::path fs_path(path);
        bool created = std::filesystem::create_directories(fs_path);
        
        if (created) {
            std::cout << "[SUCCESS] Directory created." << std::endl;
        } else {
            std::cout << "[INFO] Directory already exists." << std::endl;
        }
        
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Directory creation failed: " << e.what() << std::endl;
        return false;
    }
}