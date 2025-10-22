#include "ProjectConfig.h"

bool BuildTarget::is_valid() const {
    return !output_name.empty() && !sources.empty();
}

std::string BuildTarget::get_output_extension(const Platform& platform) const {
    Platform target_platform = platform;
    
    // If platform is unknown, detect current platform
    if (target_platform.operating_system == OS::UNKNOWN) {
        target_platform = ArchitectureDetector::detect_current_platform();
    }
    
    switch (type) {
        case BuildType::EXECUTABLE:
            return ArchitectureDetector::get_platform_executable_extension(target_platform);
        case BuildType::SHARED_LIBRARY:
            return ArchitectureDetector::get_platform_shared_library_extension(target_platform);
        case BuildType::STATIC_LIBRARY:
            return ArchitectureDetector::get_platform_static_library_extension(target_platform);
    }
    return "";
}

PlatformConfig BuildTarget::get_platform_config(const Platform& platform) const {
    PlatformConfig config(platform);
    
    // Start with base configuration
    config.cxx_flags = cxx_flags;
    config.sources = sources;
    config.include_dirs = include_dirs;
    config.library_dirs = library_dirs;
    config.libraries = libraries;
    
    // Add platform-specific compiler flags
    auto platform_flags = ArchitectureDetector::get_platform_compiler_flags(platform);
    config.cxx_flags.insert(config.cxx_flags.end(), platform_flags.begin(), platform_flags.end());
    
    // Override with platform-specific configuration if available
    for (const auto& [plat, plat_config] : platform_configs) {
        if (plat.matches(platform)) {
            // Merge configurations (platform-specific takes precedence)
            if (!plat_config.cxx_flags.empty()) {
                config.cxx_flags.insert(config.cxx_flags.end(), plat_config.cxx_flags.begin(), plat_config.cxx_flags.end());
            }
            if (!plat_config.sources.empty()) {
                config.sources.insert(config.sources.end(), plat_config.sources.begin(), plat_config.sources.end());
            }
            if (!plat_config.include_dirs.empty()) {
                config.include_dirs.insert(config.include_dirs.end(), plat_config.include_dirs.begin(), plat_config.include_dirs.end());
            }
            if (!plat_config.library_dirs.empty()) {
                config.library_dirs.insert(config.library_dirs.end(), plat_config.library_dirs.begin(), plat_config.library_dirs.end());
            }
            if (!plat_config.libraries.empty()) {
                config.libraries.insert(config.libraries.end(), plat_config.libraries.begin(), plat_config.libraries.end());
            }
            if (!plat_config.output_name_suffix.empty()) {
                config.output_name_suffix = plat_config.output_name_suffix;
            }
            break;
        }
    }
    
    return config;
}

bool BuildTarget::should_build_for_platform(const Platform& platform) const {
    // If no specific platforms are set, build for all platforms
    if (target_platforms.empty()) {
        return true;
    }
    
    // Check if platform matches any target platform
    for (const Platform& target_plat : target_platforms) {
        if (target_plat.matches(platform)) {
            return true;
        }
    }
    
    return false;
}

bool ProjectConfig::is_valid() const {
    // Check if we have at least one valid target or legacy configuration
    if (!targets.empty()) {
        for (const auto& [name, target] : targets) {
            if (target.is_valid()) {
                return !compiler.empty();
            }
        }
        return false;
    }
    
    // Legacy check
    return !compiler.empty() && !output_name.empty() && !sources.empty();
}

std::vector<std::string> ProjectConfig::get_validation_errors() const {
    std::vector<std::string> errors;
    
    // Check if we have at least one valid target or legacy configuration
    if (!targets.empty()) {
        // Multi-target mode validation
        if (compiler.empty()) {
            errors.push_back("  - Missing 'compiler' field (e.g., compiler: g++)");
        }
        
        bool has_valid_target = false;
        for (const auto& [name, target] : targets) {
            if (target.is_valid()) {
                has_valid_target = true;
                break;
            }
        }
        
        if (!has_valid_target) {
            errors.push_back("  - No valid build targets found. Each target needs:");
            
            for (const auto& [name, target] : targets) {
                if (!target.is_valid()) {
                    if (target.output_name.empty() && target.sources.empty()) {
                        errors.push_back("    * Target '" + name + "': missing both 'output_name' and 'sources'");
                    } else if (target.output_name.empty()) {
                        errors.push_back("    * Target '" + name + "': missing 'output_name'");
                    } else if (target.sources.empty()) {
                        errors.push_back("    * Target '" + name + "': missing 'sources' (e.g., " + name + ".sources: src/**.[c,cpp])");
                    }
                }
            }
        }
    } else {
        // Legacy mode validation
        if (compiler.empty()) {
            errors.push_back("  - Missing 'compiler' field (e.g., compiler: g++)");
        }
        if (output_name.empty()) {
            errors.push_back("  - Missing 'output_name' field (e.g., output_name: myapp)");
        }
        if (sources.empty()) {
            errors.push_back("  - Missing 'sources' field (e.g., sources: src/**.[c,cpp])");
        }
    }
    
    return errors;
}

void ProjectConfig::apply_defaults() {
    if (compiler.empty()) {
        compiler = "g++";
    }
    
    // If we have legacy configuration, convert it
    if (!output_name.empty() && !sources.empty() && targets.empty()) {
        convert_legacy_to_targets();
    }
    
    // Apply defaults to targets
    for (auto& [name, target] : targets) {
        if (target.output_name.empty()) {
            target.output_name = name;
        }
    }
}

void ProjectConfig::convert_legacy_to_targets() {
    BuildTarget default_target;
    default_target.name = "default";
    default_target.type = BuildType::EXECUTABLE;
    default_target.output_name = output_name;
    default_target.cxx_flags = cxx_flags;
    default_target.sources = sources;
    default_target.include_dirs = include_dirs;
    default_target.library_dirs = library_dirs;
    default_target.libraries = libraries;
    
    targets["default"] = default_target;
}