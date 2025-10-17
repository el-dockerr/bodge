#pragma once

#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <string>
#include <vector>
#include <map>

/**
 * @brief Supported operating systems
 */
enum class OS {
    WINDOWS,
    LINUX,
    UNIX,      // Generic Unix (not Linux)
    APPLE,     // macOS
    UNKNOWN
};

/**
 * @brief Supported architectures (bit width)
 */
enum class Architecture {
    X86,       // 32-bit
    X64,       // 64-bit
    ARM,       // 32-bit ARM
    ARM64,     // 64-bit ARM
    UNKNOWN
};

/**
 * @brief Platform information combining OS and architecture
 */
struct Platform {
    OS operating_system;
    Architecture architecture;
    
    Platform(OS os = OS::UNKNOWN, Architecture arch = Architecture::UNKNOWN) 
        : operating_system(os), architecture(arch) {}
    
    /**
     * @brief Get string representation of the platform
     */
    std::string to_string() const;
    
    /**
     * @brief Check if this platform matches another (supports wildcards)
     */
    bool matches(const Platform& other) const;
    
    /**
     * @brief Create platform from string representation
     */
    static Platform from_string(const std::string& platform_str);
    
    /**
     * @brief Get all possible platform combinations
     */
    static std::vector<Platform> get_all_platforms();
    
    /**
     * @brief Comparison operators for use in maps/sets
     */
    bool operator<(const Platform& other) const;
    bool operator==(const Platform& other) const;
    bool operator!=(const Platform& other) const;
};

/**
 * @brief Architecture detection and platform utilities
 */
class ArchitectureDetector {
public:
    /**
     * @brief Detect current platform automatically
     */
    static Platform detect_current_platform();
    
    /**
     * @brief Get OS string representation
     */
    static std::string os_to_string(OS os);
    
    /**
     * @brief Get architecture string representation
     */
    static std::string architecture_to_string(Architecture arch);
    
    /**
     * @brief Parse OS from string
     */
    static OS string_to_os(const std::string& os_str);
    
    /**
     * @brief Parse architecture from string
     */
    static Architecture string_to_architecture(const std::string& arch_str);
    
    /**
     * @brief Get platform-specific compiler flags
     */
    static std::vector<std::string> get_platform_compiler_flags(const Platform& platform);
    
    /**
     * @brief Get platform-specific file extensions
     */
    static std::string get_platform_executable_extension(const Platform& platform);
    static std::string get_platform_shared_library_extension(const Platform& platform);
    static std::string get_platform_static_library_extension(const Platform& platform);
};

/**
 * @brief Platform-specific configuration
 */
struct PlatformConfig {
    Platform target_platform;
    std::vector<std::string> cxx_flags;
    std::vector<std::string> sources;
    std::vector<std::string> include_dirs;
    std::vector<std::string> library_dirs;
    std::vector<std::string> libraries;
    std::string output_name_suffix;  // e.g., "_x64", "_win32"
    
    PlatformConfig() = default;
    PlatformConfig(const Platform& platform) : target_platform(platform) {}
};

#endif // ARCHITECTURE_H