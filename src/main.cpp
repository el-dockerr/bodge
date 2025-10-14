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
        bool success = true;

        // Check for command line arguments
        if (argc > 1) {
            std::string command = argv[1];
            
            if (command == "help" || command == "--help" || command == "-h") {
                std::cout << "Bodge - The Idiotic Build System\n"
                          << "Usage: bodge [command] [target/sequence]\n\n"
                          << "Commands:\n"
                          << "  build [target]     - Build specific target (default: all targets)\n"
                          << "  fetch              - Fetch git dependencies only\n"
                          << "  sequence [name]    - Execute specific sequence\n"
                          << "  list               - List available targets and sequences\n"
                          << "  help               - Show this help message\n\n"
                          << "  version            - Show version information\n\n"
                          << "Examples:\n"
                          << "  bodge                    # Build all targets\n"
                          << "  bodge fetch              # Fetch git dependencies\n"
                          << "  bodge build mylib        # Build target 'mylib'\n"
                          << "  bodge sequence deploy    # Execute sequence 'deploy'\n";
                return 0;
            } else if (command == "version" || command == "--version" || command == "-v") {
                std::cout << "Bodge - The Idiotic Build System\n"
                          << "Version: " << get_version() << "\n";
                return 0;
            } else if (command == "list") {
                std::cout << "Available targets:\n";
                for (const auto& [name, target] : project.targets) {
                    std::string type_str;
                    switch (target.type) {
                        case BuildType::EXECUTABLE: type_str = "exe"; break;
                        case BuildType::SHARED_LIBRARY: type_str = "shared"; break;
                        case BuildType::STATIC_LIBRARY: type_str = "static"; break;
                    }
                    std::cout << "  " << name << " (" << type_str << ")\n";
                }
                
                std::cout << "\nAvailable sequences:\n";
                for (const auto& [name, seq] : project.sequences) {
                    std::cout << "  " << name << " (" << seq.operations.size() << " operations)\n";
                }
                return 0;
            } else if (command == "fetch") {
                // Fetch git dependencies only
                success = builder.build_git_dependencies_only();
            } else if (command == "build") {
                if (argc > 2) {
                    // Build specific target
                    success = builder.build_target(argv[2]);
                } else {
                    // Build all targets
                    success = builder.build();
                }
            } else if (command == "sequence") {
                if (argc > 2) {
                    // Execute specific sequence
                    success = builder.execute_sequence(argv[2]);
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
            success = builder.build();
        }

        return success ? 0 : 1;
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