#include "ConfigParser.h"
#include "StringUtils.h"
#include "FileSystemUtils.h"
#include <fstream>
#include <iostream>

ProjectConfig ConfigParser::load_project_config(const std::string& filename) {
    ProjectConfig config;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "[FATAL] Could not open configuration file: " << filename 
                  << ". Check file existence and path." << std::endl;
        return config; // Return empty config
    }

    std::string line;
    while (std::getline(file, line)) {
        process_config_line(line, config);
    }

    // Apply default values for missing configuration
    config.apply_defaults();
    
    return config;
}

void ConfigParser::process_config_line(const std::string& line, ProjectConfig& config) {
    // Ignore comments (#) and empty lines
    if (line.empty() || line[0] == '#') {
        return;
    }

    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
        return; // Skip malformed lines
    }

    std::string key = StringUtils::trim(line.substr(0, colon_pos));
    std::string value_str = StringUtils::trim(line.substr(colon_pos + 1));

    if (key.empty() || value_str.empty()) {
        return;
    }

    // Check for platform-specific configuration [target@platform.property] or [@platform.property]
    if (key.find('@') != std::string::npos) {
        process_platform_config_line(key, value_str, config);
        return;
    }

    // Check for sequence configuration [sequence.name]
    // This must come before the general target check since "sequence." also contains a dot
    if (key.find("sequence.") == 0) {
        process_sequence_config_line(key, value_str, config);
        return;
    }
    
    // Check for target-specific configuration [target.property]
    if (key.find('.') != std::string::npos) {
        process_target_config_line(key, value_str, config);
        return;
    }

    // Global configuration
    if (key == "name") {
        config.name = value_str;
    } else if (key == "compiler") {
        config.compiler = value_str;
    } else if (key == "global_cxx_flags") {
        config.global_cxx_flags = StringUtils::split(value_str, ',');
    } else if (key == "global_include_dirs") {
        config.global_include_dirs = StringUtils::split(value_str, ',');
    } else if (key == "global_library_dirs") {
        config.global_library_dirs = StringUtils::split(value_str, ',');
    } else if (key == "global_libraries") {
        config.global_libraries = StringUtils::split(value_str, ',');
    } else if (key == "dependencies_url") {
        config.dependencies_url = StringUtils::split(value_str, ',');
    } else if (key == "dependencies_path") {
        config.dependencies_path = StringUtils::split(value_str, ',');
    } else if (key == "run_bodge_after_clone") {
        config.run_bodge_after_clone = value_str;
    }
    // Legacy support
    else if (key == "output_name") {
        config.output_name = value_str;
    } else if (key == "cxx_flags") {
        config.cxx_flags = StringUtils::split(value_str, ',');
    } else if (key == "sources") {
        std::vector<std::string> raw_sources = StringUtils::split(value_str, ',');
        config.sources = expand_sources(raw_sources);
    } else if (key == "include_dirs") {
        config.include_dirs = StringUtils::split(value_str, ',');
    } else if (key == "library_dirs") {
        config.library_dirs = StringUtils::split(value_str, ',');
    } else if (key == "libraries") {
        config.libraries = StringUtils::split(value_str, ',');
    } else if (key == "platforms") {
        // Set default target platforms
        std::vector<std::string> platform_strings = StringUtils::split(value_str, ',');
        for (const std::string& platform_str : platform_strings) {
            Platform platform = Platform::from_string(StringUtils::trim(platform_str));
            if (platform.operating_system != OS::UNKNOWN || platform.architecture != Architecture::UNKNOWN) {
                config.default_target_platforms.push_back(platform);
            }
        }
    }
}

void ConfigParser::process_target_config_line(const std::string& key, const std::string& value, ProjectConfig& config) {
    size_t dot_pos = key.find('.');
    std::string target_name = key.substr(0, dot_pos);
    std::string property = key.substr(dot_pos + 1);
    
    // Create target if it doesn't exist
    if (config.targets.find(target_name) == config.targets.end()) {
        config.targets[target_name] = BuildTarget();
        config.targets[target_name].name = target_name;
        config.targets[target_name].type = BuildType::EXECUTABLE; // Default
    }
    
    BuildTarget& target = config.targets[target_name];
    
    if (property == "type") {
        target.type = parse_build_type(value);
    } else if (property == "output_name") {
        target.output_name = value;
    } else if (property == "sources") {
        std::vector<std::string> raw_sources = StringUtils::split(value, ',');
        target.sources = expand_sources(raw_sources);
    } else if (property == "cxx_flags") {
        target.cxx_flags = StringUtils::split(value, ',');
    } else if (property == "include_dirs") {
        target.include_dirs = StringUtils::split(value, ',');
    } else if (property == "library_dirs") {
        target.library_dirs = StringUtils::split(value, ',');
    } else if (property == "libraries") {
        target.libraries = StringUtils::split(value, ',');
    } else if (property == "platforms") {
        // Set target platforms
        std::vector<std::string> platform_strings = StringUtils::split(value, ',');
        for (const std::string& platform_str : platform_strings) {
            Platform platform = Platform::from_string(StringUtils::trim(platform_str));
            if (platform.operating_system != OS::UNKNOWN || platform.architecture != Architecture::UNKNOWN) {
                target.target_platforms.push_back(platform);
            }
        }
    }
}

void ConfigParser::process_sequence_config_line(const std::string& key, const std::string& value, ProjectConfig& config) {
    std::string seq_name = key.substr(9); // Remove "sequence."
    
    // Create sequence if it doesn't exist
    if (config.sequences.find(seq_name) == config.sequences.end()) {
        config.sequences[seq_name] = Sequence();
        config.sequences[seq_name].name = seq_name;
    }
    
    // Parse operations (format: "build:target1 copy:src->dst remove:file mkdir:dir")
    std::vector<std::string> ops = StringUtils::split(value, ' ');
    
    for (const std::string& op : ops) {
        size_t colon_pos = op.find(':');
        if (colon_pos == std::string::npos) continue;
        
        std::string op_type = op.substr(0, colon_pos);
        std::string op_params = op.substr(colon_pos + 1);
        
        Operation operation;
        
        if (op_type == "build") {
            operation.type = OperationType::BUILD;
            operation.target = op_params;
        } else if (op_type == "copy") {
            operation.type = OperationType::COPY;
            size_t arrow_pos = op_params.find("->");
            if (arrow_pos != std::string::npos) {
                operation.target = StringUtils::trim(op_params.substr(0, arrow_pos));
                operation.destination = StringUtils::trim(op_params.substr(arrow_pos + 2));
            }
        } else if (op_type == "remove") {
            operation.type = OperationType::REMOVE;
            operation.target = op_params;
        } else if (op_type == "mkdir") {
            operation.type = OperationType::MKDIR;
            operation.target = op_params;
        }
        
        config.sequences[seq_name].operations.push_back(operation);
    }
}

BuildType ConfigParser::parse_build_type(const std::string& type_str) {
    if (type_str == "exe" || type_str == "executable") {
        return BuildType::EXECUTABLE;
    } else if (type_str == "dll" || type_str == "so" || type_str == "shared") {
        return BuildType::SHARED_LIBRARY;
    } else if (type_str == "lib" || type_str == "static") {
        return BuildType::STATIC_LIBRARY;
    }
    return BuildType::EXECUTABLE; // Default
}

void ConfigParser::process_platform_config_line(const std::string& key, const std::string& value, ProjectConfig& config) {
    size_t at_pos = key.find('@');
    std::string target_name = (at_pos > 0) ? key.substr(0, at_pos) : "";
    std::string platform_and_property = key.substr(at_pos + 1);
    
    size_t dot_pos = platform_and_property.find('.');
    if (dot_pos == std::string::npos) {
        return; // Malformed platform configuration
    }
    
    std::string platform_str = platform_and_property.substr(0, dot_pos);
    std::string property = platform_and_property.substr(dot_pos + 1);
    
    Platform platform = Platform::from_string(platform_str);
    if (platform.operating_system == OS::UNKNOWN && platform.architecture == Architecture::UNKNOWN) {
        return; // Invalid platform
    }
    
    if (target_name.empty()) {
        // Global platform configuration [@platform.property]
        if (config.global_platform_configs.find(platform) == config.global_platform_configs.end()) {
            config.global_platform_configs[platform] = PlatformConfig(platform);
        }
        
        PlatformConfig& plat_config = config.global_platform_configs[platform];
        apply_platform_property(plat_config, property, value);
    } else {
        // Target-specific platform configuration [target@platform.property]
        if (config.targets.find(target_name) == config.targets.end()) {
            config.targets[target_name] = BuildTarget();
            config.targets[target_name].name = target_name;
            config.targets[target_name].type = BuildType::EXECUTABLE; // Default
        }
        
        BuildTarget& target = config.targets[target_name];
        
        if (target.platform_configs.find(platform) == target.platform_configs.end()) {
            target.platform_configs[platform] = PlatformConfig(platform);
        }
        
        PlatformConfig& plat_config = target.platform_configs[platform];
        apply_platform_property(plat_config, property, value);
    }
}

void ConfigParser::apply_platform_property(PlatformConfig& plat_config, const std::string& property, const std::string& value) {
    if (property == "cxx_flags") {
        plat_config.cxx_flags = StringUtils::split(value, ',');
    } else if (property == "sources") {
        std::vector<std::string> raw_sources = StringUtils::split(value, ',');
        plat_config.sources = expand_sources(raw_sources);
    } else if (property == "include_dirs") {
        plat_config.include_dirs = StringUtils::split(value, ',');
    } else if (property == "library_dirs") {
        plat_config.library_dirs = StringUtils::split(value, ',');
    } else if (property == "libraries") {
        plat_config.libraries = StringUtils::split(value, ',');
    } else if (property == "output_suffix") {
        plat_config.output_name_suffix = value;
    }
}

std::vector<std::string> ConfigParser::expand_sources(const std::vector<std::string>& sources) {
    return FileSystemUtils::expand_source_patterns(sources);
}