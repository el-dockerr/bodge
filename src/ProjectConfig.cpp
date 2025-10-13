#include "ProjectConfig.h"

bool BuildTarget::is_valid() const {
    return !output_name.empty() && !sources.empty();
}

std::string BuildTarget::get_output_extension() const {
#ifdef _WIN32
    switch (type) {
        case BuildType::EXECUTABLE:
            return ".exe";
        case BuildType::SHARED_LIBRARY:
            return ".dll";
        case BuildType::STATIC_LIBRARY:
            return ".lib";
    }
#else
    switch (type) {
        case BuildType::EXECUTABLE:
            return "";
        case BuildType::SHARED_LIBRARY:
            return ".so";
        case BuildType::STATIC_LIBRARY:
            return ".a";
    }
#endif
    return "";
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