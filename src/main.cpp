/**
 * Bodge - The Idiotic Build System
 * A minimalistic but powerful C++ build system that reads from a simple configuration file
 * 
 * Author: Swen "El Dockerr" Kalski
 * Email: kalski.swen@gmail.com
 * License: GNU General Public License v3.0
 */

#include "ConfigParser.h"
#include "BuildSystem.h"
#include "Architecture.h"
#include "core.h"
#include <iostream>

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
            args.platform = Platform::from_string(platform_str);
            args.platform_specified = true;
        } else if (arg.find("--arch=") == 0) {
            std::string arch_str = arg.substr(7); // Remove "--arch="
            Architecture arch = ArchitectureDetector::string_to_architecture(arch_str);
            if (arch != Architecture::UNKNOWN) {
                // Keep current OS, change architecture
                args.platform.architecture = arch;
                args.arch_specified = true;
            }
        } else if (arg.find("--interval=") == 0) {
            std::string interval_str = arg.substr(11); // Remove "--interval="
            try {
                args.poll_interval = std::stoi(interval_str);
                if (args.poll_interval < 100) {
                    args.poll_interval = 100; // Minimum 100ms
                }
            } catch (...) {
                std::cerr << "[WARNING] Invalid interval value, using default (1000ms)" << std::endl;
            }
        } else if (arg.find("--log=") == 0) {
            args.log_file = arg.substr(6); // Remove "--log="
        } else if (arg.find("--") == 0) {
            // Skip other unknown options
            continue;
        } else if (args.command.empty()) {
            args.command = arg;
        } else if (args.target_or_sequence.empty()) {
            args.target_or_sequence = arg;
        }
    }
    
    return args;
}

int main(int argc, char* argv[]) {
    try {
        // Load configuration from the file (defaults to .bodge)
        ProjectConfig project = ConfigParser::load_project_config(".bodge");

        // Fatal check if essential information is missing after loading
        if (!project.is_valid()) {
            std::cerr << "[FATAL] Configuration is critically incomplete. "
                      << "Please ensure required fields are set in .bodge." 
                      << std::endl;
            return 1;
        }

        // Create build system
        BuildSystem builder(project);
        E_RESULT result;

        // Parse command line arguments
        CommandLineArgs args = parse_command_line(argc, argv);

        // Check for command line arguments
        if (argc > 1 && !args.command.empty()) {
            std::cout << "Bodge - The Idiotic Build System" << std::endl;
            
            if (args.platform_specified || args.arch_specified) {
                std::cout << "Target platform: " << args.platform.to_string() << std::endl << std::endl;
            }
            
            if (args.command == "help" || args.command == "--help" || args.command == "-h") {
                std::cout << "Usage: bodge [command] [target/sequence] [options]" << std::endl << std::endl
                          << "Commands:" << std::endl
                          << "  build [target]     - Build specific target (default: all targets)" << std::endl
                          << "  fetch              - Fetch git dependencies only" << std::endl
                          << "  sequence [name]    - Execute specific sequence" << std::endl
                          << "  watch              - Watch mode: automatically rebuild on file changes" << std::endl
                          << "  daemon             - Alias for watch mode" << std::endl
                          << "  list               - List available targets and sequences" << std::endl
                          << "  platform           - Show current platform information" << std::endl
                          << "  help               - Show this help message" << std::endl
                          << "  version            - Show version information" << std::endl << std::endl
                          << "Options:" << std::endl
                          << "  --platform=<platform>  - Build for specific platform" << std::endl
                          << "  --arch=<arch>          - Build for specific architecture" << std::endl
                          << "  --interval=<ms>        - Poll interval for watch mode (default: 1000ms)" << std::endl
                          << "  --log=<file>           - Log file for watch mode (default: bodge_daemon.log)" << std::endl << std::endl
                          << "Examples:" << std::endl
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
                std::cout << "Author: Swen \"El Dockerr\" Kalski" << std::endl
                          << "Version: " << get_version() << std::endl;
                return 0;
            } else if (args.command == "platform") {
                Platform current_platform = ArchitectureDetector::detect_current_platform();
                std::cout << "Current platform information:" << std::endl;
                std::cout << "  OS: " << ArchitectureDetector::os_to_string(current_platform.operating_system) << std::endl;
                std::cout << "  Architecture: " << ArchitectureDetector::architecture_to_string(current_platform.architecture) << std::endl;
                std::cout << "  Platform string: " << current_platform.to_string() << std::endl << std::endl;
                
                std::cout << "Supported platforms:" << std::endl;
                std::vector<Platform> all_platforms = Platform::get_all_platforms();
                for (const Platform& platform : all_platforms) {
                    std::cout << "  " << platform.to_string() << std::endl;
                }
                return 0;
            } else if (args.command == "list") {
                std::cout << "Available targets:" << std::endl;
                for (const auto& [name, target] : project.targets) {
                    std::string type_str;
                    switch (target.type) {
                        case BuildType::EXECUTABLE: type_str = "exe"; break;
                        case BuildType::SHARED_LIBRARY: type_str = "shared"; break;
                        case BuildType::STATIC_LIBRARY: type_str = "static"; break;
                    }
                    std::cout << "  " << name << " (" << type_str << ")";
                    
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

                std::cout << std::endl << "Available sequences:" << std::endl;
                for (const auto& [name, seq] : project.sequences) {
                    std::cout << "  " << name << " (" << seq.operations.size() << " operations)" << std::endl;
                }
                
                // Show configured target platforms
                if (!project.default_target_platforms.empty()) {
                    std::cout << std::endl << "Default target platforms:" << std::endl;
                    for (const Platform& platform : project.default_target_platforms) {
                        std::cout << "  " << platform.to_string() << std::endl;
                    }
                }
                
                return 0;
            } else if (args.command == "fetch") {
                // Fetch git dependencies only
                result = builder.build_git_dependencies_only();
            } else if (args.command == "watch" || args.command == "daemon") {
                // Run in daemon/watch mode
                result = builder.run_daemon_mode(args.poll_interval, args.log_file);
            } else if (args.command == "build") {
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
                    // Execute specific sequence
                    result = builder.execute_sequence(args.target_or_sequence);
                } else {
                    std::cerr << "[ERROR] Please specify a sequence name." << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] Unknown command: " << args.command << std::endl;
                std::cerr << "Use 'bodge help' for usage information." << std::endl;
                return 1;
            }
        } else {
            // Default behavior or platform-only arguments
            if (args.platform_specified || args.arch_specified) {
                std::cout << "Bodge - The Idiotic Build System" << std::endl;
                std::cout << "Target platform: " << args.platform.to_string() << std::endl << std::endl;
                
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