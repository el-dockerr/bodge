#pragma once

#include <string>
#include <vector>
#include <map>

/**
 * @brief Types of build outputs supported
 */
enum class BuildType {
    EXECUTABLE,
    SHARED_LIBRARY,  // .dll on Windows, .so on Linux
    STATIC_LIBRARY   // .lib/.a
};

/**
 * @brief Types of operations supported in sequences
 */
enum class OperationType {
    BUILD,
    COPY,
    REMOVE,
    MKDIR
};

/**
 * @brief Represents a single operation in a build sequence
 */
struct Operation {
    OperationType type;
    std::string target;      // For BUILD: target name, for others: source path
    std::string destination; // For COPY: destination path
    
    Operation() = default;
    Operation(OperationType t, const std::string& tgt, const std::string& dst = "") 
        : type(t), target(tgt), destination(dst) {}
};

/**
 * @brief Represents a build target (executable, library, etc.)
 */
struct BuildTarget {
    std::string name;
    BuildType type;
    std::string output_name;
    std::vector<std::string> cxx_flags;
    std::vector<std::string> sources;
    std::vector<std::string> include_dirs;
    std::vector<std::string> library_dirs;
    std::vector<std::string> libraries;
    
    /**
     * @brief Checks if the target has minimum required fields
     */
    bool is_valid() const;
    
    /**
     * @brief Gets the appropriate file extension for the build type
     */
    std::string get_output_extension() const;
};

/**
 * @brief Represents a sequence of operations
 */
struct Sequence {
    std::string name;
    std::vector<Operation> operations;
};

/**
 * @brief Enhanced configuration structure that supports multiple targets and sequences
 */
struct ProjectConfig {
    std::string name;
    std::string compiler;
    
    // Global defaults
    std::vector<std::string> global_cxx_flags;
    std::vector<std::string> global_include_dirs;
    std::vector<std::string> global_library_dirs;
    std::vector<std::string> global_libraries;

    // Global git paths
    std::vector<std::string> dependencies_url; // URLs of git repositories
    std::vector<std::string> dependencies_path; // Local paths for git repositories
    std::string run_bodge_after_clone; // Command to run bodge after cloning - e.g., "bodge" - when it is available
    
    // Build targets
    std::map<std::string, BuildTarget> targets;
    
    // Build sequences
    std::map<std::string, Sequence> sequences;
    
    // Legacy support - converted to default target
    std::string output_name;
    std::vector<std::string> cxx_flags;
    std::vector<std::string> sources;
    std::vector<std::string> include_dirs;
    std::vector<std::string> library_dirs;
    std::vector<std::string> libraries;

    /**
     * @brief Checks if the configuration has the minimum required fields
     * @return true if configuration is valid, false otherwise
     */
    bool is_valid() const;

    /**
     * @brief Applies default values for missing configuration fields
     */
    void apply_defaults();
    
    /**
     * @brief Converts legacy configuration to modern target-based format
     */
    void convert_legacy_to_targets();
};