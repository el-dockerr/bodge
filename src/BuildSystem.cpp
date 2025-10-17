#include "BuildSystem.h"
#include "StringUtils.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

BuildSystem::BuildSystem(const ProjectConfig& config) : config_(config) {}

E_RESULT BuildSystem::build() const {
    std::cout << "--- Bodge: The Idiotic Build System ---" << std::endl;

    // Validate system support
    if (!validate_system_support()) {
        std::cerr << "System command execution is not fully supported on this platform. " << std::endl
                  << "Build may fail." << std::endl;
    }

    // Check for essential inputs
    if (!config_.is_valid()) {
        std::cerr << "[ERROR] Configuration is invalid. Please check your .bodge file." << std::endl;
        return S_ERROR_INVALID_ARGUMENT;
    }

    // Handle git dependencies first
    if (build_git_dependencies() != S_OK) {
        std::cerr << "[ERROR] Failed to handle git dependencies." << std::endl;
        return S_BUILD_FAILED;
    }

    // If we have targets, build all of them for all target platforms
    if (!config_.targets.empty()) {
        bool all_success = true;
        std::vector<Platform> target_platforms = get_target_platforms();
        
        for (const Platform& platform : target_platforms) {
            std::cout << std::endl << "[INFO] Building for platform: " << platform.to_string() << std::endl;
            
            for (const auto& [name, target] : config_.targets) {
                if (target.should_build_for_platform(platform)) {
                    if (build_target_for_platform(name, platform) != S_OK) {
                        all_success = false;
                    }
                } else {
                    std::cout << "[SKIP] Target '" << name << "' not configured for platform " << platform.to_string() << std::endl;
                }
            }
        }
        return all_success ? S_OK : S_BUILD_FAILED;
    }
    
    // Legacy single build
    std::string command = generate_command();
    return execute_command(command);
}

E_RESULT BuildSystem::build_git_dependencies_only() const {
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

E_RESULT BuildSystem::execute_command(const std::string& command) const {
    std::cout << "\n[INFO] Executing command (The Idiot Way):" << std::endl << command  << std::endl;

    // Execute the command using the system shell
    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << std::endl << "[SUCCESS] Project '" << config_.output_name << "' built successfully." << std::endl;
        return S_OK;
    } else {
        std::cerr << std::endl << "[ERROR] Bodge encountered an issue (Exit Code: " << result
                  << "). Check the compiler output for details." << std::endl;
        return S_COMMAND_EXECUTION_FAILED;
    }
}

E_RESULT BuildSystem::validate_system_support() const {
    return std::system(nullptr) != 0 ? S_OK : S_FAILURE;
}

E_RESULT BuildSystem::build_target(const std::string& target_name) const {
    // Build for current platform by default
    Platform current_platform = ArchitectureDetector::detect_current_platform();
    return build_target_for_platform(target_name, current_platform);
}

E_RESULT BuildSystem::build_target_for_platform(const std::string& target_name, const Platform& platform) const {
    auto it = config_.targets.find(target_name);
    if (it == config_.targets.end()) {
        std::cerr << "[ERROR] Target '" << target_name << "' not found." << std::endl;
        return S_TARGET_NOT_FOUND;
    }
    
    const BuildTarget& target = it->second;
    if (!target.is_valid()) {
        std::cerr << "[ERROR] Target '" << target_name << "' is invalid." << std::endl;
        return S_INVALID_CONFIGURATION;
    }

    if (!target.should_build_for_platform(platform)) {
        std::cout << "[SKIP] Target '" << target_name << "' not configured for platform " << platform.to_string() << std::endl;
        return S_OK;
    }

    std::cout << std::endl << "[INFO] Building target: " << target_name << " for platform: " << platform.to_string() << std::endl;
    std::string command = generate_target_command_for_platform(target, platform);
    return execute_command(command);
}

E_RESULT BuildSystem::execute_sequence(const std::string& sequence_name) const {
    auto it = config_.sequences.find(sequence_name);
    if (it == config_.sequences.end()) {
        std::cerr << "[ERROR] Sequence '" << sequence_name << "' not found." << std::endl;
        return S_ERROR_RESOURCE_NOT_FOUND;
    }
    
    const Sequence& sequence = it->second;
    std::cout << std::endl << "[INFO] Executing sequence: " << sequence_name << std::endl;
    
    for (const Operation& op : sequence.operations) {
        if (execute_operation(op) != S_OK) {
            std::cerr << "[ERROR] Sequence '" << sequence_name << "' failed." << std::endl;
            return S_FAILURE;
        }
    }
    
    std::cout << "[SUCCESS] Sequence '" << sequence_name << "' completed successfully." << std::endl;
    return S_OK;
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

E_RESULT BuildSystem::build_git_dependencies() const {
    if (config_.dependencies_url.empty() && config_.dependencies_path.empty()) {
        return S_OK; // No dependencies to handle
    }
    if (config_.dependencies_url.size() != config_.dependencies_path.size()) {
        std::cerr << "[ERROR] Mismatch between number of dependency git URLs and git paths." << std::endl;
        return S_ERROR_INVALID_ARGUMENT;
    }

    Git git;
    int i = 0;
    
    for (const auto& url : config_.dependencies_url) {
        std::cout << "[INFO] Cloning dependency from " << url << std::endl;
        try {
            E_RESULT res = git.manage_git_repository(url, config_.dependencies_path[i]);
            if (res != S_OK) {
                std::cerr << "[ERROR] Failed to clone " << url << std::endl;
                return S_FAILURE;
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
                                return S_COMMAND_EXECUTION_FAILED;
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
                            return S_COMMAND_EXECUTION_FAILED;
                        }
                    }
                }
                i++;
            } else {
                std::cerr << "[ERROR] Failed to clone " << url << std::endl;
                return S_FAILURE;
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception during git operation: " << e.what() << std::endl;
            return S_FAILURE;
        }
    }
    return S_OK;
}

E_RESULT BuildSystem::execute_operation(const Operation& operation) const {
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
    return S_FAILURE; // Unknown operation type
}

E_RESULT BuildSystem::copy_file_or_directory(const std::string& source, const std::string& destination) const {
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
        return S_OK;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Copy failed: " << e.what() << std::endl;
        return S_COMMAND_EXECUTION_FAILED;
    }
}

E_RESULT BuildSystem::remove_file_or_directory(const std::string& path) const {
    std::cout << "[INFO] Removing " << path << std::endl;
    
    try {
        std::filesystem::path fs_path(path);
        std::uintmax_t removed_count = std::filesystem::remove_all(fs_path);
        
        if (removed_count > 0) {
            std::cout << "[SUCCESS] Removed " << removed_count << " item(s)." << std::endl;
        } else {
            std::cout << "[INFO] Nothing to remove (path doesn't exist)." << std::endl;
        }

        return S_OK;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Remove failed: " << e.what() << std::endl;
        return S_COMMAND_EXECUTION_FAILED;
    }
}

E_RESULT BuildSystem::create_directory(const std::string& path) const {
    std::cout << "[INFO] Creating directory " << path << std::endl;
    
    try {
        std::filesystem::path fs_path(path);
        bool created = std::filesystem::create_directories(fs_path);
        
        if (created) {
            std::cout << "[SUCCESS] Directory created." << std::endl;
        } else {
            std::cout << "[INFO] Directory already exists." << std::endl;
        }

        return S_OK;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Directory creation failed: " << e.what() << std::endl;
        return S_COMMAND_EXECUTION_FAILED;
    }
}

std::string BuildSystem::generate_target_command_for_platform(const BuildTarget& target, const Platform& platform) const {
    // Get platform-specific configuration
    PlatformConfig platform_config = target.get_platform_config(platform);
    
    std::stringstream command;
    
    // 1. Compiler
    command << config_.compiler;
    
    // 2. Global CXX Flags
    command << " " << StringUtils::join(config_.global_cxx_flags, "", " ");
    
    // 3. Global platform-specific flags
    auto global_plat_it = config_.global_platform_configs.find(platform);
    if (global_plat_it != config_.global_platform_configs.end()) {
        command << " " << StringUtils::join(global_plat_it->second.cxx_flags, "", " ");
    }
    
    // 4. Target-specific CXX Flags (including platform-specific)
    command << " " << StringUtils::join(platform_config.cxx_flags, "", " ");
    
    // 5. Build type specific flags
    switch (target.type) {
        case BuildType::SHARED_LIBRARY:
            if (platform.operating_system == OS::WINDOWS) {
                command << " -shared";
            } else {
                command << " -shared -fPIC";
            }
            break;
        case BuildType::STATIC_LIBRARY:
            // Note: Static libraries need different handling (ar command)
            // For now, we'll treat them as objects that need to be archived
            break;
        case BuildType::EXECUTABLE:
            // No special flags needed
            break;
    }
    
    // 6. Global Include Directories (-I)
    command << StringUtils::join(config_.global_include_dirs, " -I", " ");
    
    // 7. Global platform-specific include directories
    if (global_plat_it != config_.global_platform_configs.end()) {
        command << StringUtils::join(global_plat_it->second.include_dirs, " -I", " ");
    }
    
    // 8. Platform-specific Include Directories (-I)
    command << StringUtils::join(platform_config.include_dirs, " -I", " ");
    
    // 9. Platform-specific Source Files
    command << StringUtils::join(platform_config.sources, "", " ");
    
    // 10. Output file (-o) with platform-specific suffix
    std::string output_name = target.output_name + platform_config.output_name_suffix + target.get_output_extension(platform);
    command << " -o " << output_name;
    
    // 11. Global Library Directories (-L)
    command << StringUtils::join(config_.global_library_dirs, " -L", " ");
    
    // 12. Global platform-specific library directories
    if (global_plat_it != config_.global_platform_configs.end()) {
        command << StringUtils::join(global_plat_it->second.library_dirs, " -L", " ");
    }
    
    // 13. Platform-specific Library Directories (-L)
    command << StringUtils::join(platform_config.library_dirs, " -L", " ");
    
    // 14. Global Libraries (-l)
    command << StringUtils::join(config_.global_libraries, " -l", " ");
    
    // 15. Global platform-specific libraries
    if (global_plat_it != config_.global_platform_configs.end()) {
        command << StringUtils::join(global_plat_it->second.libraries, " -l", " ");
    }
    
    // 16. Platform-specific Libraries (-l)
    command << StringUtils::join(platform_config.libraries, " -l", " ");
    
    return command.str();
}

std::vector<Platform> BuildSystem::get_target_platforms() const {
    std::vector<Platform> platforms;
    
    // If default target platforms are configured, use those
    if (!config_.default_target_platforms.empty()) {
        platforms = config_.default_target_platforms;
    } else {
        // Otherwise, use current platform
        platforms.push_back(ArchitectureDetector::detect_current_platform());
    }
    
    return platforms;
}