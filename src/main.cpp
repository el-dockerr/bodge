/**
 * Bodge - The Idiotic Build System
 * A minimalistic but powerful C++ build system that reads from a simple configuration file
 * 
 * Author: Swen "El Dockerr" Kalski
 * Email: kalski.swen@gmail.com
 * License: Bodge License (BL-V1.0)
 */

#include "ConfigParser.h"
#include "BuildSystem.h"
#include "Architecture.h"
#include "ProgressBar.h"
#include "Strings.h"
#include "core.h"
#include <iostream>
#include <cstdlib>

// Helper structure to hold parsed command line arguments
struct CommandLineArgs {
    std::string command;
    std::string target_or_sequence;
    Platform platform;
    bool platform_specified = false;
    bool arch_specified = false;
    int poll_interval = 1000;  // Default poll interval for daemon mode (ms)
    std::string log_file = "bodge_daemon.log";  // Default log file for daemon mode
};

// Function to parse command line arguments
CommandLineArgs parse_command_line(int argc, char* argv[]) {
    CommandLineArgs args;
    
    // Default to current platform
    args.platform = ArchitectureDetector::detect_current_platform();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.find("--platform=") == 0) {
            std::string platform_str = arg.substr(11); // Remove "--platform="
            if (platform_str.empty()) {
                std::cerr << STR(WARN_EMPTY_PLATFORM) << std::endl;
                continue;
            }
            args.platform = Platform::from_string(platform_str);
            args.platform_specified = true;
        } else if (arg.find("--arch=") == 0) {
            std::string arch_str = arg.substr(7); // Remove "--arch="
            if (arch_str.empty()) {
                std::cerr << STR(WARN_EMPTY_ARCH) << std::endl;
                continue;
            }
            Architecture arch = ArchitectureDetector::string_to_architecture(arch_str);
            if (arch != Architecture::UNKNOWN) {
                // Keep current OS, change architecture
                args.platform.architecture = arch;
                args.arch_specified = true;
            } else {
                std::cerr << STR(WARN_UNKNOWN_ARCH) << arch_str << std::endl;
            }
        } else if (arg.find("--interval=") == 0) {
            std::string interval_str = arg.substr(11); // Remove "--interval="
            if (interval_str.empty()) {
                std::cerr << STR(WARN_EMPTY_INTERVAL) << std::endl;
                continue;
            }
            try {
                // Use strtol for better error checking and overflow protection
                char* end_ptr;
                long value = std::strtol(interval_str.c_str(), &end_ptr, 10);
                
                // Check for conversion errors
                if (end_ptr == interval_str.c_str() || *end_ptr != '\0') {
                    std::cerr << STR(WARN_INVALID_INTERVAL) << std::endl;
                    continue;
                }
                
                // Check for overflow and range
                if (value < 100 || value > 3600000) { // Max 1 hour
                    std::cerr << STR(WARN_INTERVAL_OUT_OF_RANGE) << std::endl;
                    continue;
                }
                
                args.poll_interval = static_cast<int>(value);
            } catch (...) {
                std::cerr << STR(WARN_INVALID_INTERVAL) << std::endl;
            }
        } else if (arg.find("--log=") == 0) {
            std::string log_file = arg.substr(6); // Remove "--log="
            if (log_file.empty()) {
                std::cerr << STR(WARN_EMPTY_LOG_FILE) << std::endl;
                continue;
            }
            // Validate log file path
            if (log_file.find("..") != std::string::npos || log_file.length() > 256) {
                std::cerr << STR(WARN_INVALID_LOG_FILE) << std::endl;
                continue;
            }
            args.log_file = log_file;
        } else if (arg.find("--") == 0) {
            // Skip other unknown options
            std::cerr << STR(WARN_UNKNOWN_OPTION) << arg << std::endl;
            continue;
        } else if (args.command.empty()) {
            args.command = arg;
        } else if (args.target_or_sequence.empty()) {
            args.target_or_sequence = arg;
        }
    }
    
    return args;
}

void projectLoadError() {
    std::cerr << STR(ERR_CONFIG_INCOMPLETE) << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Initialize internationalization
        Strings::initialize();
        
        E_RESULT result;

        // Parse command line arguments
        CommandLineArgs args = parse_command_line(argc, argv);

        // Check for command line arguments
        if (argc > 1 && !args.command.empty()) {
            std::cout << STR(APP_TITLE) << std::endl;
            
            if (args.platform_specified || args.arch_specified) {
                std::cout << STR(MSG_TARGET_PLATFORM) << args.platform.to_string() << std::endl << std::endl;
            }
            
            if (args.command == "help" || args.command == "--help" || args.command == "-h") {
                std::cout << STR(CMD_USAGE) << std::endl << std::endl
                          << STR(CMD_COMMANDS) << std::endl
                          << STR(CMD_BUILD) << std::endl
                          << STR(CMD_FETCH) << std::endl
                          << STR(CMD_SEQUENCE) << std::endl
                          << STR(CMD_WATCH) << std::endl
                          << STR(CMD_DAEMON) << std::endl
                          << STR(CMD_LIST) << std::endl
                          << STR(CMD_PLATFORM) << std::endl
                          << STR(CMD_HELP) << std::endl
                          << STR(CMD_VERSION) << std::endl << std::endl
                          << STR(OPT_OPTIONS) << std::endl
                          << STR(OPT_PLATFORM) << std::endl
                          << STR(OPT_ARCH) << std::endl
                          << STR(OPT_INTERVAL) << std::endl
                          << STR(OPT_LOG) << std::endl << std::endl
                          << STR(EXAMPLES_HEADER) << std::endl
                          << "  bodge                          # Build all targets for current platform" << std::endl
                          << "  bodge --platform=linux_x64    # Build all targets for Linux 64-bit" << std::endl
                          << "  bodge build mylib --arch=x86   # Build 'mylib' for 32-bit" << std::endl
                          << "  bodge build myapp --platform=windows_x64  # Build 'myapp' for Windows 64-bit" << std::endl
                          << "  bodge fetch                    # Fetch git dependencies" << std::endl
                          << "  bodge sequence deploy          # Execute sequence 'deploy'" << std::endl
                          << "  bodge watch                    # Watch for file changes and auto-rebuild" << std::endl
                          << "  bodge daemon --interval=2000   # Watch mode with 2s poll interval" << std::endl;
                return 0;
            } else if (args.command == "version" || args.command == "--version" || args.command == "-v") {
                ProgressBar::display_header();
                ProgressBar::display_info(STR(APP_AUTHOR));
                ProgressBar::display_info(STR(APP_VERSION) + std::string(get_version()));
                return 0;
            } else if (args.command == "platform") {
                Platform current_platform = ArchitectureDetector::detect_current_platform();
                std::cout << STR(MSG_CURRENT_PLATFORM_INFO) << std::endl;
                std::cout << STR(MSG_OS) << ArchitectureDetector::os_to_string(current_platform.operating_system) << std::endl;
                std::cout << STR(MSG_ARCHITECTURE) << ArchitectureDetector::architecture_to_string(current_platform.architecture) << std::endl;
                std::cout << STR(MSG_PLATFORM_STRING) << current_platform.to_string() << std::endl << std::endl;
                
                std::cout << STR(MSG_SUPPORTED_PLATFORMS) << std::endl;
                std::vector<Platform> all_platforms = Platform::get_all_platforms();
                for (const Platform& platform : all_platforms) {
                    std::cout << "  " << platform.to_string() << std::endl;
                }
                return 0;
            } else if (args.command == "list") {
                // Load configuration from the file (defaults to .bodge)
                ProjectConfig project = ConfigParser::load_project_config(".bodge");

                // Fatal check if essential information is missing after loading
                if (!project.is_valid()) {
                    projectLoadError();
                    return 1;
                }

                // Create build system
                BuildSystem builder(project);
                ProgressBar::display_phase_header("Available Targets", "🎯");
                for (const auto& [name, target] : project.targets) {
                    std::string type_str;
                    switch (target.type) {
                        case BuildType::EXECUTABLE: type_str = "exe"; break;
                        case BuildType::SHARED_LIBRARY: type_str = "shared"; break;
                        case BuildType::STATIC_LIBRARY: type_str = "static"; break;
                    }
                    std::cout << "  • " << name << " (" << type_str << ")";
                    
                    // Show target platforms if configured
                    if (!target.target_platforms.empty()) {
                        std::cout << " - platforms: ";
                        for (size_t i = 0; i < target.target_platforms.size(); ++i) {
                            if (i > 0) std::cout << ", ";
                            std::cout << target.target_platforms[i].to_string();
                        }
                    }
                    std::cout << std::endl;
                }

                ProgressBar::display_phase_header("Available Sequences", "⚙️");
                for (const auto& [name, seq] : project.sequences) {
                    std::cout << "  • " << name << " (" << seq.operations.size() << " operations)" << std::endl;
                }
                
                // Show configured target platforms
                if (!project.default_target_platforms.empty()) {
                    ProgressBar::display_phase_header("Default Target Platforms", "🖥️");
                    for (const Platform& platform : project.default_target_platforms) {
                        std::cout << "  • " << platform.to_string() << std::endl;
                    }
                }
                
                return 0;
            } else if (args.command == "fetch") {
                ProgressBar::display_header();
                // Fetch git dependencies only
                // Load configuration from the file (defaults to .bodge)
                ProjectConfig project = ConfigParser::load_project_config(".bodge");

                // Fatal check if essential information is missing after loading
                if (!project.is_valid()) {
                    projectLoadError();
                    return 1;
                }

                // Create build system
                BuildSystem builder(project);
                result = builder.build_git_dependencies_only();
            } else if (args.command == "watch" || args.command == "daemon") {
                // Load configuration from the file (defaults to .bodge)
                ProjectConfig project = ConfigParser::load_project_config(".bodge");

                // Fatal check if essential information is missing after loading
                if (!project.is_valid()) {
                    projectLoadError();
                    return 1;
                }

                // Create build system
                BuildSystem builder(project);
                // Run in daemon/watch mode
                result = builder.run_daemon_mode(args.poll_interval, args.log_file);
            } else if (args.command == "build") {
                // Load configuration from the file (defaults to .bodge)
                ProjectConfig project = ConfigParser::load_project_config(".bodge");

                // Fatal check if essential information is missing after loading
                if (!project.is_valid()) {
                    projectLoadError();
                    return 1;
                }

                // Create build system
                BuildSystem builder(project);
                if (!args.target_or_sequence.empty()) {
                    // Build specific target for specified platform
                    result = builder.build_target_for_platform(args.target_or_sequence, args.platform);
                } else {
                    // Build all targets for specified platform
                    if (args.platform_specified || args.arch_specified) {
                        // Build all targets for the specified platform
                        bool all_success = true;
                        for (const auto& [name, target] : project.targets) {
                            if (target.should_build_for_platform(args.platform)) {
                                if (builder.build_target_for_platform(name, args.platform) != S_OK) {
                                    all_success = false;
                                }
                            }
                        }
                        result = all_success ? S_OK : S_BUILD_FAILED;
                    } else {
                        // Use default build behavior
                        result = builder.build();
                    }
                }
            } else if (args.command == "sequence") {
                if (!args.target_or_sequence.empty()) {
                    // Load configuration from the file (defaults to .bodge)
                    ProjectConfig project = ConfigParser::load_project_config(".bodge");

                    // Fatal check if essential information is missing after loading
                    if (!project.is_valid()) {
                        projectLoadError();
                        return 1;
                    }

                    // Create build system
                    BuildSystem builder(project);
                    // Execute specific sequence
                    result = builder.execute_sequence(args.target_or_sequence);
                } else {
                    std::cerr << STR(ERR_SPECIFY_SEQUENCE) << std::endl;
                    return 1;
                }
            } else {
                std::cerr << STR(ERR_UNKNOWN_COMMAND) << args.command << std::endl;
                std::cerr << STR(ERR_USE_HELP) << std::endl;
                return 1;
            }
        } else {
            // Default behavior or platform-only arguments
            if (args.platform_specified || args.arch_specified) {
                std::cout << STR(APP_TITLE) << std::endl;
                std::cout << STR(MSG_TARGET_PLATFORM) << args.platform.to_string() << std::endl << std::endl;
                // Load configuration from the file (defaults to .bodge)
                ProjectConfig project = ConfigParser::load_project_config(".bodge");

                // Fatal check if essential information is missing after loading
                if (!project.is_valid()) {
                    projectLoadError();
                    return 1;
                }

                // Create build system
                BuildSystem builder(project);
                // Build all targets for the specified platform
                bool all_success = true;
                for (const auto& [name, target] : project.targets) {
                    if (target.should_build_for_platform(args.platform)) {
                        if (builder.build_target_for_platform(name, args.platform) != S_OK) {
                            all_success = false;
                        }
                    }
                }
                result = all_success ? S_OK : S_BUILD_FAILED;
            } else {
                // Load configuration from the file (defaults to .bodge)
                ProjectConfig project = ConfigParser::load_project_config(".bodge");

                // Fatal check if essential information is missing after loading
                if (!project.is_valid()) {
                    projectLoadError();
                    return 1;
                }

                // Create build system
                BuildSystem builder(project);
                result = builder.build();
            }
        }

        return result == S_OK ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "[FATAL] An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "[FATAL] An unknown error occurred." << std::endl;
        return 1;
    }
}