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

        // Check for command line arguments
        if (argc > 1) {
            std::string command = argv[1];

            std::cout << "Bodge - The Idiotic Build System" << std::endl;
            
            if (command == "help" || command == "--help" || command == "-h") {
                std::cout << "Usage: bodge [command] [target/sequence]" << std::endl << std::endl
                          << "Commands:" << std::endl
                          << "  build [target]     - Build specific target (default: all targets)" << std::endl
                          << "  fetch              - Fetch git dependencies only" << std::endl
                          << "  sequence [name]    - Execute specific sequence" << std::endl
                          << "  list               - List available targets and sequences" << std::endl
                          << "  platform           - Show current platform information" << std::endl
                          << "  help               - Show this help message" << std::endl << std::endl
                          << "  version            - Show version information" << std::endl << std::endl
                          << "Examples:" << std::endl
                          << "  bodge                    # Build all targets" << std::endl
                          << "  bodge fetch              # Fetch git dependencies" << std::endl
                          << "  bodge build mylib        # Build target 'mylib'" << std::endl
                          << "  bodge sequence deploy    # Execute sequence 'deploy'" << std::endl;
                return 0;
            } else if (command == "version" || command == "--version" || command == "-v") {
                std::cout << "Bodge - The Idiotic Build System" << std::endl
                          << "Author: Swen \"El Dockerr\" Kalski" << std::endl
                          << "Version: " << get_version() << std::endl;
                return 0;
            } else if (command == "platform") {
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
            } else if (command == "list") {
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
            } else if (command == "fetch") {
                // Fetch git dependencies only
                result = builder.build_git_dependencies_only();
            } else if (command == "build") {
                if (argc > 2) {
                    // Build specific target
                    result = builder.build_target(argv[2]);
                } else {
                    // Build all targets
                    result = builder.build();
                }
            } else if (command == "sequence") {
                if (argc > 2) {
                    // Execute specific sequence
                    result = builder.execute_sequence(argv[2]);
                } else {
                    std::cerr << "[ERROR] Please specify a sequence name." << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] Unknown command: " << command << std::endl;
                std::cerr << "Use 'bodge help' for usage information." << std::endl;
                return 1;
            }
        } else {
            // Default: build all targets
            result = builder.build();
        }

        return result == S_OK;
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