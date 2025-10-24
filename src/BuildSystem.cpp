#include "BuildSystem.h"
#include "StringUtils.h"
#include "FileWatcher.h"
#include "BuildLogger.h"
#include "FileSystemUtils.h"
#include "ProgressBar.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <thread>
#include <chrono>
#include <csignal>
#include <algorithm>
#include <regex>

namespace {
    // Helper function to validate that a string doesn't contain command injection attempts
    bool is_safe_compiler_argument(const std::string& arg) {
        // Check for dangerous patterns
        const std::vector<std::string> dangerous_patterns = {
            ";", "&&", "||", "|", "`", "$(", "${", "\n", "\r"
        };
        
        for (const auto& pattern : dangerous_patterns) {
            if (arg.find(pattern) != std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
    
    // Validate all arguments in a vector
    bool validate_compiler_arguments(const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (!is_safe_compiler_argument(arg)) {
                std::cerr << "[ERROR] Potentially dangerous compiler argument detected: " << arg << std::endl;
                return false;
            }
        }
        return true;
    }
}

BuildSystem::BuildSystem(const ProjectConfig& config) : config_(config) {}

E_RESULT BuildSystem::prebuild_checks() const{
    // Check for essential inputs
    if (!config_.is_valid()) {
        ProgressBar::display_error("Configuration is invalid. Please check your .bodge file.");
        return S_ERROR_INVALID_ARGUMENT;
    }

    // Handle git dependencies first
    if (build_git_dependencies() != S_OK) {
        ProgressBar::display_error("Failed to handle git dependencies.");
        return S_BUILD_FAILED;
    }

    return S_OK;
}


E_RESULT BuildSystem::build() const {
    ProgressBar::display_header();

    E_RESULT prechk_result = prebuild_checks();
    if (prechk_result != S_OK) {
        return prechk_result;
    }

    auto build_start_time = std::chrono::steady_clock::now();

    // Handle git dependencies first
    if (build_git_dependencies() != S_OK) {
        ProgressBar::display_error("Failed to handle git dependencies.");
        return S_BUILD_FAILED;
    }

    // If we have targets, build all of them for all target platforms
    if (!config_.targets.empty()) {
        bool all_success = true;
        std::vector<Platform> target_platforms = get_target_platforms();
        
        // Count total targets to build
        int total_targets = 0;
        for (const Platform& platform : target_platforms) {
            for (const auto& [name, target] : config_.targets) {
                if (target.should_build_for_platform(platform)) {
                    total_targets++;
                }
            }
        }
        
        int current_target = 0;
        ProgressBar overall_progress(total_targets, 50);
        
        for (const Platform& platform : target_platforms) {
            ProgressBar::display_info("Building for platform: " + platform.to_string());
            
            for (const auto& [name, target] : config_.targets) {
                if (target.should_build_for_platform(platform)) {
                    current_target++;
                    std::string prefix = "Overall progress (" + std::to_string(current_target) + "/" + std::to_string(total_targets) + ")";
                    overall_progress.display(current_target, prefix);
                    
                    if (build_target_for_platform(name, platform) != S_OK) {
                        all_success = false;
                    }
                } else {
                    ProgressBar::display_info("Skipping target '" + name + "' - not configured for platform " + platform.to_string());
                }
            }
        }
        
        auto build_end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(build_end_time - build_start_time).count();
        
        ProgressBar::display_build_summary(all_success, duration);
        return all_success ? S_OK : S_BUILD_FAILED;
    }
    
    // Legacy single build
    std::string command = generate_command();
    E_RESULT result = execute_command(command);
    
    auto build_end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(build_end_time - build_start_time).count();
    
    ProgressBar::display_build_summary(result == S_OK, duration);
    return result;
}

E_RESULT BuildSystem::build_git_dependencies_only() const {
    return build_git_dependencies();
}

std::string BuildSystem::generate_command() const {
    // Validate inputs before generating command
    if (!is_safe_compiler_argument(config_.compiler)) {
        ProgressBar::display_error("Invalid compiler specified");
        return "";
    }
    
    if (!validate_compiler_arguments(config_.cxx_flags) ||
        !validate_compiler_arguments(config_.include_dirs) ||
        !validate_compiler_arguments(config_.sources) ||
        !validate_compiler_arguments(config_.library_dirs) ||
        !validate_compiler_arguments(config_.libraries)) {
        ProgressBar::display_error("Invalid compiler arguments detected");
        return "";
    }
    
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
    if (!is_safe_compiler_argument(config_.output_name)) {
        ProgressBar::display_error("Invalid output name specified");
        return "";
    }
    command << " -o " << config_.output_name;

    // 6. Library Directories (-L)
    command << StringUtils::join(config_.library_dirs, " -L", " ");

    // 7. Libraries (-l)
    command << StringUtils::join(config_.libraries, " -l", " ");

    return command.str();
}

E_RESULT BuildSystem::execute_command(const std::string& command) const {
    // Validate command is not empty (could indicate validation failure)
    if (command.empty()) {
        ProgressBar::display_error("Build command is empty or invalid");
        return S_ERROR_INVALID_ARGUMENT;
    }
    
    // Basic sanity check on command length
    if (command.length() > 32768) { // 32KB max command length
        ProgressBar::display_error("Build command is too long");
        return S_ERROR_INVALID_ARGUMENT;
    }
    
    ProgressBar::display_info("Executing build command...");
    std::cout << command << std::endl;

    // Execute the command using the system shell
    int result = std::system(command.c_str());

    if (result == 0) {
        ProgressBar::display_success("Build completed successfully!");
        return S_OK;
    } else {
        ProgressBar::display_error("Build failed with exit code: " + std::to_string(result));
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

    E_RESULT prechk_result = prebuild_checks();
    if (prechk_result != S_OK) {
        return prechk_result;
    }

    auto it = config_.targets.find(target_name);
    if (it == config_.targets.end()) {
        ProgressBar::display_error("Target '" + target_name + "' not found.");
        return S_TARGET_NOT_FOUND;
    }
    
    const BuildTarget& target = it->second;
    if (!target.is_valid()) {
        ProgressBar::display_error("Target '" + target_name + "' is invalid.");
        return S_INVALID_CONFIGURATION;
    }

    if (!target.should_build_for_platform(platform)) {
        ProgressBar::display_info("Skipping target '" + target_name + "' - not configured for platform " + platform.to_string());
        return S_OK;
    }

    ProgressBar::display_phase_header("Building: " + target_name + " (" + platform.to_string() + ")", "🔨");
    
    auto build_start = std::chrono::steady_clock::now();
    std::string command = generate_target_command_for_platform(target, platform);
    E_RESULT result = execute_command(command);
    auto build_end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(build_end - build_start).count();
    
    if (result == S_OK) {
        ProgressBar::display_success("Target '" + target_name + "' built in " + std::to_string(duration) + "ms");
    }
    
    return result;
}

E_RESULT BuildSystem::execute_sequence(const std::string& sequence_name) const {
    auto it = config_.sequences.find(sequence_name);
    if (it == config_.sequences.end()) {
        ProgressBar::display_error("Sequence '" + sequence_name + "' not found.");
        return S_ERROR_RESOURCE_NOT_FOUND;
    }
    
    const Sequence& sequence = it->second;
    ProgressBar::display_phase_header("Executing Sequence: " + sequence_name, "⚙️");
    
    int total_ops = sequence.operations.size();
    ProgressBar seq_progress(total_ops, 50);
    int current_op = 0;
    
    for (const Operation& op : sequence.operations) {
        seq_progress.display(current_op, "Sequence progress");
        current_op++;
        
        if (execute_operation(op) != S_OK) {
            ProgressBar::display_error("Sequence '" + sequence_name + "' failed at operation " + std::to_string(current_op));
            return S_FAILURE;
        }
    }
    
    seq_progress.display(total_ops, "Sequence progress");
    ProgressBar::display_success("Sequence '" + sequence_name + "' completed successfully!");
    return S_OK;
}

std::string BuildSystem::generate_target_command(const BuildTarget& target) const {
    // Validate inputs
    if (!is_safe_compiler_argument(config_.compiler)) {
        ProgressBar::display_error("Invalid compiler specified");
        return "";
    }
    
    if (!validate_compiler_arguments(config_.global_cxx_flags) ||
        !validate_compiler_arguments(target.cxx_flags) ||
        !validate_compiler_arguments(config_.global_include_dirs) ||
        !validate_compiler_arguments(target.include_dirs) ||
        !validate_compiler_arguments(target.sources) ||
        !validate_compiler_arguments(config_.global_library_dirs) ||
        !validate_compiler_arguments(target.library_dirs) ||
        !validate_compiler_arguments(config_.global_libraries) ||
        !validate_compiler_arguments(target.libraries)) {
        ProgressBar::display_error("Invalid compiler arguments detected in target");
        return "";
    }
    
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
    if (!is_safe_compiler_argument(output_name)) {
        ProgressBar::display_error("Invalid output name specified");
        return "";
    }
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
        ProgressBar::display_error("Mismatch between number of dependency git URLs and git paths.");
        return S_ERROR_INVALID_ARGUMENT;
    }

    ProgressBar::display_phase_header("Fetching Dependencies", "📦");
    
    Git git;
    size_t total_deps = config_.dependencies_url.size();
    ProgressBar dep_progress(static_cast<int>(total_deps), 50);
    
    for (size_t i = 0; i < total_deps; ++i) {
        // Bounds check (should be redundant due to size check above, but defensive)
        if (i >= config_.dependencies_url.size() || i >= config_.dependencies_path.size()) {
            ProgressBar::display_error("Index out of bounds in dependencies arrays.");
            return S_ERROR_INVALID_ARGUMENT;
        }
        
        const std::string& url = config_.dependencies_url[i];
        const std::string& path = config_.dependencies_path[i];
        
        std::string short_url = url.length() > 50 ? url.substr(0, 47) + "..." : url;
        ProgressBar::display_info("Fetching: " + short_url);
        dep_progress.display(static_cast<int>(i), "Dependencies");
        
        try {
            E_RESULT res = git.manage_git_repository(url, path);
            if (res != S_OK && res != S_GIT_ERROR) {
                ProgressBar::display_error("Failed to fetch " + url + " (error: " + std::to_string(res) + ")");
                return S_FAILURE;
            }
            
            ProgressBar::display_success("Fetched " + short_url);
            
            if (!config_.run_bodge_after_clone.empty() && config_.run_bodge_after_clone == "true") {
                ProgressBar::display_info("Running post-clone command in " + path);
                
                // Save current directory
                std::filesystem::path original_path;
                try {
                    original_path = std::filesystem::current_path();
                } catch (const std::filesystem::filesystem_error& e) {
                    ProgressBar::display_error("Failed to get current directory: " + std::string(e.what()));
                    return S_COMMAND_EXECUTION_FAILED;
                }
                
                try {
                    // Change to the cloned repository directory
                    std::filesystem::current_path(path);
                    
                    // Run bodge in the repository directory
                    int result = std::system("bodge");
                    
                    // Restore original directory immediately
                    std::filesystem::current_path(original_path);
                    
                    if (result != 0) {
                        ProgressBar::display_error("Post-clone bodge command failed with exit code: " + std::to_string(result));
                        return S_COMMAND_EXECUTION_FAILED;
                    }
                    
                    ProgressBar::display_success("Post-clone bodge command completed.");
                } catch (const std::filesystem::filesystem_error& e) {
                    ProgressBar::display_error("Failed to change directory: " + std::string(e.what()));
                    // Attempt to restore original directory
                    try {
                        std::filesystem::current_path(original_path);
                    } catch (...) {
                        ProgressBar::display_error("Failed to restore original directory!");
                    }
                    return S_COMMAND_EXECUTION_FAILED;
                }
            }
        } catch (const std::exception& e) {
            ProgressBar::display_error("Exception during git operation: " + std::string(e.what()));
            return S_FAILURE;
        }
    }
    
    dep_progress.display(static_cast<int>(total_deps), "Dependencies");
    ProgressBar::display_success("All dependencies fetched successfully!");
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
    ProgressBar::display_info("Copying " + source + " -> " + destination);
    
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
        
        ProgressBar::display_success("Copy completed.");
        return S_OK;
    } catch (const std::filesystem::filesystem_error& e) {
        ProgressBar::display_error("Copy failed: " + std::string(e.what()));
        return S_COMMAND_EXECUTION_FAILED;
    }
}

E_RESULT BuildSystem::remove_file_or_directory(const std::string& path) const {
    ProgressBar::display_info("Removing " + path);
    
    try {
        std::filesystem::path fs_path(path);
        std::uintmax_t removed_count = std::filesystem::remove_all(fs_path);
        
        if (removed_count > 0) {
            ProgressBar::display_success("Removed " + std::to_string(removed_count) + " item(s).");
        } else {
            ProgressBar::display_info("Nothing to remove (path doesn't exist).");
        }

        return S_OK;
    } catch (const std::filesystem::filesystem_error& e) {
        ProgressBar::display_error("Remove failed: " + std::string(e.what()));
        return S_COMMAND_EXECUTION_FAILED;
    }
}

E_RESULT BuildSystem::create_directory(const std::string& path) const {
    ProgressBar::display_info("Creating directory " + path);
    
    try {
        std::filesystem::path fs_path(path);
        bool created = std::filesystem::create_directories(fs_path);
        
        if (created) {
            ProgressBar::display_success("Directory created.");
        } else {
            ProgressBar::display_info("Directory already exists.");
        }

        return S_OK;
    } catch (const std::filesystem::filesystem_error& e) {
        ProgressBar::display_error("Directory creation failed: " + std::string(e.what()));
        return S_COMMAND_EXECUTION_FAILED;
    }
}

std::string BuildSystem::generate_target_command_for_platform(const BuildTarget& target, const Platform& platform) const {
    // Validate compiler
    if (!is_safe_compiler_argument(config_.compiler)) {
        ProgressBar::display_error("Invalid compiler specified");
        return "";
    }
    
    // Get platform-specific configuration
    PlatformConfig platform_config = target.get_platform_config(platform);
    
    // Validate all arguments
    if (!validate_compiler_arguments(config_.global_cxx_flags) ||
        !validate_compiler_arguments(platform_config.cxx_flags) ||
        !validate_compiler_arguments(config_.global_include_dirs) ||
        !validate_compiler_arguments(platform_config.include_dirs) ||
        !validate_compiler_arguments(platform_config.sources) ||
        !validate_compiler_arguments(config_.global_library_dirs) ||
        !validate_compiler_arguments(platform_config.library_dirs) ||
        !validate_compiler_arguments(config_.global_libraries) ||
        !validate_compiler_arguments(platform_config.libraries)) {
        ProgressBar::display_error("Invalid compiler arguments detected in platform-specific target");
        return "";
    }
    
    std::stringstream command;
    
    // 1. Compiler
    command << config_.compiler;
    
    // 2. Global CXX Flags
    command << " " << StringUtils::join(config_.global_cxx_flags, "", " ");
    
    // 3. Global platform-specific flags
    auto global_plat_it = config_.global_platform_configs.find(platform);
    if (global_plat_it != config_.global_platform_configs.end()) {
        if (!validate_compiler_arguments(global_plat_it->second.cxx_flags)) {
            ProgressBar::display_error("Invalid global platform-specific compiler arguments");
            return "";
        }
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
        if (!validate_compiler_arguments(global_plat_it->second.include_dirs)) {
            ProgressBar::display_error("Invalid global platform-specific include directories");
            return "";
        }
        command << StringUtils::join(global_plat_it->second.include_dirs, " -I", " ");
    }
    
    // 8. Platform-specific Include Directories (-I)
    command << StringUtils::join(platform_config.include_dirs, " -I", " ");
    
    // 9. Platform-specific Source Files
    command << StringUtils::join(platform_config.sources, "", " ");
    
    // 10. Output file (-o) with platform-specific suffix
    std::string output_name = target.output_name + platform_config.output_name_suffix + target.get_output_extension(platform);
    if (!is_safe_compiler_argument(output_name)) {
        ProgressBar::display_error("Invalid output name specified");
        return "";
    }
    command << " -o " << output_name;
    
    // 11. Global Library Directories (-L)
    command << StringUtils::join(config_.global_library_dirs, " -L", " ");
    
    // 12. Global platform-specific library directories
    if (global_plat_it != config_.global_platform_configs.end()) {
        if (!validate_compiler_arguments(global_plat_it->second.library_dirs)) {
            ProgressBar::display_error("Invalid global platform-specific library directories");
            return "";
        }
        command << StringUtils::join(global_plat_it->second.library_dirs, " -L", " ");
    }
    
    // 13. Platform-specific Library Directories (-L)
    command << StringUtils::join(platform_config.library_dirs, " -L", " ");
    
    // 14. Global Libraries (-l)
    command << StringUtils::join(config_.global_libraries, " -l", " ");
    
    // 15. Global platform-specific libraries
    if (global_plat_it != config_.global_platform_configs.end()) {
        if (!validate_compiler_arguments(global_plat_it->second.libraries)) {
            ProgressBar::display_error("Invalid global platform-specific libraries");
            return "";
        }
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

// Global flag for daemon shutdown
static volatile bool daemon_should_stop = false;

// Signal handler for graceful shutdown
static void daemon_signal_handler(int signal) {
    (void)signal; // Unused parameter
    daemon_should_stop = true;
}

E_RESULT BuildSystem::run_daemon_mode(int poll_interval_ms, const std::string& log_file) const {
    std::cout << "--- Bodge Daemon Mode ---" << std::endl;
    
    // Check configuration validity
    if (!config_.is_valid()) {
        std::cerr << "[ERROR] Configuration is invalid. Please check your .bodge file." << std::endl;
        return S_ERROR_INVALID_ARGUMENT;
    }

    // Initialize build logger
    BuildLogger logger(log_file);
    if (!logger.open()) {
        std::cerr << "[ERROR] Failed to open log file: " << log_file << std::endl;
        return S_FILE_OPERATION_FAILED;
    }

    std::cout << "[INFO] Build logs will be written to: " << log_file << std::endl;
    logger.log_message("Daemon mode started");

    // Collect all source files and directories to watch
    std::vector<std::string> watch_paths;
    
    // Add global include directories
    for (const std::string& dir : config_.global_include_dirs) {
        if (FileSystemUtils::directory_exists(dir)) {
            watch_paths.push_back(dir);
        }
    }
    
    // Add target-specific sources and includes
    for (const auto& [name, target] : config_.targets) {
        // Add source files
        for (const std::string& source : target.sources) {
            watch_paths.push_back(source);
        }
        
        // Add include directories
        for (const std::string& dir : target.include_dirs) {
            if (FileSystemUtils::directory_exists(dir)) {
                watch_paths.push_back(dir);
            }
        }
        
        // Add platform-specific sources
        for (const auto& [platform, platform_config] : target.platform_configs) {
            for (const std::string& source : platform_config.sources) {
                watch_paths.push_back(source);
            }
        }
    }

    // Add legacy sources if present
    for (const std::string& source : config_.sources) {
        watch_paths.push_back(source);
    }
    
    // Add legacy include directories
    for (const std::string& dir : config_.include_dirs) {
        if (FileSystemUtils::directory_exists(dir)) {
            watch_paths.push_back(dir);
        }
    }

    // If no specific paths, watch current directory
    if (watch_paths.empty()) {
        watch_paths.push_back(".");
    }

    // Initialize file watcher
    FileWatcher watcher(watch_paths);
    if (!watcher.initialize()) {
        std::cerr << "[ERROR] Failed to initialize file watcher." << std::endl;
        logger.log_error("Failed to initialize file watcher");
        return S_FAILURE;
    }

    std::cout << "[INFO] Watching " << watcher.get_watched_files().size() << " file(s) for changes..." << std::endl;
    std::cout << "[INFO] Poll interval: " << poll_interval_ms << " ms" << std::endl;
    std::cout << "[INFO] Press Ctrl+C to stop daemon mode." << std::endl;

    // Set up signal handler for graceful shutdown
    std::signal(SIGINT, daemon_signal_handler);
    std::signal(SIGTERM, daemon_signal_handler);

    // Perform initial build
    std::cout << std::endl << "[INFO] Performing initial build..." << std::endl;
    logger.log_message("Performing initial build");
    logger.log_build_start();
    
    auto start_time = std::chrono::steady_clock::now();
    E_RESULT build_result = build();
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    logger.log_build_end(build_result, duration);
    
    if (build_result == S_OK) {
        std::cout << "[SUCCESS] Initial build completed successfully." << std::endl;
    } else {
        std::cerr << "[ERROR] Initial build failed. Continuing to watch for changes..." << std::endl;
    }

    // Main daemon loop
    std::cout << std::endl << "[INFO] Daemon is now watching for file changes..." << std::endl;
    
    while (!daemon_should_stop) {
        // Wait for the specified interval
        std::this_thread::sleep_for(std::chrono::milliseconds(poll_interval_ms));

        // Check for file changes
        if (watcher.has_changes()) {
            std::vector<std::string> changed_files = watcher.get_changed_files();
            
            std::cout << std::endl << "[CHANGE DETECTED] " << changed_files.size() 
                      << " file(s) changed:" << std::endl;
            logger.log_changed_files(changed_files);
            
            for (const std::string& file : changed_files) {
                std::cout << "  - " << file << std::endl;
            }

            // Small delay to allow file writes to complete
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Trigger rebuild
            std::cout << std::endl << "[INFO] Triggering rebuild..." << std::endl;
            logger.log_build_start();
            
            start_time = std::chrono::steady_clock::now();
            build_result = build();
            end_time = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            logger.log_build_end(build_result, duration);
            
            if (build_result == S_OK) {
                std::cout << "[SUCCESS] Rebuild completed successfully." << std::endl;
            } else {
                std::cerr << "[ERROR] Rebuild failed. See log for details." << std::endl;
            }

            // Update file watcher state
            watcher.update_state();
            
            std::cout << std::endl << "[INFO] Watching for file changes..." << std::endl;
        }
    }

    // Daemon was interrupted
    std::cout << std::endl << "[INFO] Daemon mode stopped." << std::endl;
    logger.log_message("Daemon mode stopped by user");

    return S_OK;
}