#include "Architecture.h"
#include <algorithm>
#include <cctype>

// Platform detection includes
#ifdef _WIN32
    #include <windows.h>
#elif defined(__linux__)
    #include <sys/utsname.h>
#elif defined(__unix__) || defined(__APPLE__)
    #include <sys/utsname.h>
#endif

std::string Platform::to_string() const {
    std::string result;
    
    // Add OS
    switch (operating_system) {
        case OS::WINDOWS: result += "windows"; break;
        case OS::LINUX: result += "linux"; break;
        case OS::UNIX: result += "unix"; break;
        case OS::APPLE: result += "apple"; break;
        case OS::UNKNOWN: result += "unknown"; break;
    }
    
    result += "_";
    
    // Add architecture
    switch (architecture) {
        case Architecture::X86: result += "x86"; break;
        case Architecture::X64: result += "x64"; break;
        case Architecture::ARM: result += "arm"; break;
        case Architecture::ARM64: result += "arm64"; break;
        case Architecture::UNKNOWN: result += "unknown"; break;
    }
    
    return result;
}

bool Platform::matches(const Platform& other) const {
    // Support wildcards (UNKNOWN matches any)
    bool os_match = (operating_system == OS::UNKNOWN || 
                     other.operating_system == OS::UNKNOWN || 
                     operating_system == other.operating_system);
                     
    bool arch_match = (architecture == Architecture::UNKNOWN || 
                       other.architecture == Architecture::UNKNOWN || 
                       architecture == other.architecture);
                       
    return os_match && arch_match;
}

Platform Platform::from_string(const std::string& platform_str) {
    std::string lower_str = platform_str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    Platform platform;
    
    // Parse OS
    if (lower_str.find("windows") != std::string::npos || lower_str.find("win") != std::string::npos) {
        platform.operating_system = OS::WINDOWS;
    } else if (lower_str.find("linux") != std::string::npos) {
        platform.operating_system = OS::LINUX;
    } else if (lower_str.find("unix") != std::string::npos) {
        platform.operating_system = OS::UNIX;
    } else if (lower_str.find("apple") != std::string::npos || lower_str.find("macos") != std::string::npos || lower_str.find("darwin") != std::string::npos) {
        platform.operating_system = OS::APPLE;
    } else {
        platform.operating_system = OS::UNKNOWN;
    }
    
    // Parse architecture
    if (lower_str.find("x64") != std::string::npos || lower_str.find("x86_64") != std::string::npos || lower_str.find("amd64") != std::string::npos) {
        platform.architecture = Architecture::X64;
    } else if (lower_str.find("x86") != std::string::npos || lower_str.find("i386") != std::string::npos || lower_str.find("i686") != std::string::npos) {
        platform.architecture = Architecture::X86;
    } else if (lower_str.find("arm64") != std::string::npos || lower_str.find("aarch64") != std::string::npos) {
        platform.architecture = Architecture::ARM64;
    } else if (lower_str.find("arm") != std::string::npos) {
        platform.architecture = Architecture::ARM;
    } else {
        platform.architecture = Architecture::UNKNOWN;
    }
    
    return platform;
}

std::vector<Platform> Platform::get_all_platforms() {
    std::vector<Platform> platforms;
    
    // Generate all combinations
    std::vector<OS> operating_systems = {OS::WINDOWS, OS::LINUX, OS::UNIX, OS::APPLE};
    std::vector<Architecture> architectures = {Architecture::X86, Architecture::X64, Architecture::ARM, Architecture::ARM64};
    
    for (OS os : operating_systems) {
        for (Architecture arch : architectures) {
            platforms.emplace_back(os, arch);
        }
    }
    
    return platforms;
}

Platform ArchitectureDetector::detect_current_platform() {
    Platform platform;
    
    // Detect OS
#ifdef _WIN32
    platform.operating_system = OS::WINDOWS;
#elif defined(__linux__)
    platform.operating_system = OS::LINUX;
#elif defined(__APPLE__)
    platform.operating_system = OS::APPLE;
#elif defined(__unix__)
    platform.operating_system = OS::UNIX;
#else
    platform.operating_system = OS::UNKNOWN;
#endif

    // Detect architecture
#ifdef _WIN32
    // Windows-specific detection
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    
    switch (si.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            platform.architecture = Architecture::X64;
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            platform.architecture = Architecture::X86;
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            platform.architecture = Architecture::ARM;
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            platform.architecture = Architecture::ARM64;
            break;
        default:
            platform.architecture = Architecture::UNKNOWN;
            break;
    }
#else
    // Unix-like systems
    struct utsname un;
    if (uname(&un) == 0) {
        std::string machine = un.machine;
        std::transform(machine.begin(), machine.end(), machine.begin(), ::tolower);
        
        if (machine.find("x86_64") != std::string::npos || machine.find("amd64") != std::string::npos) {
            platform.architecture = Architecture::X64;
        } else if (machine.find("i386") != std::string::npos || machine.find("i686") != std::string::npos || machine.find("x86") != std::string::npos) {
            platform.architecture = Architecture::X86;
        } else if (machine.find("aarch64") != std::string::npos || machine.find("arm64") != std::string::npos) {
            platform.architecture = Architecture::ARM64;
        } else if (machine.find("arm") != std::string::npos) {
            platform.architecture = Architecture::ARM;
        } else {
            platform.architecture = Architecture::UNKNOWN;
        }
    } else {
        platform.architecture = Architecture::UNKNOWN;
    }
#endif

    return platform;
}

std::string ArchitectureDetector::os_to_string(OS os) {
    switch (os) {
        case OS::WINDOWS: return "windows";
        case OS::LINUX: return "linux";
        case OS::UNIX: return "unix";
        case OS::APPLE: return "apple";
        case OS::UNKNOWN: return "unknown";
        default: return "unknown";
    }
}

std::string ArchitectureDetector::architecture_to_string(Architecture arch) {
    switch (arch) {
        case Architecture::X86: return "x86";
        case Architecture::X64: return "x64";
        case Architecture::ARM: return "arm";
        case Architecture::ARM64: return "arm64";
        case Architecture::UNKNOWN: return "unknown";
        default: return "unknown";
    }
}

OS ArchitectureDetector::string_to_os(const std::string& os_str) {
    std::string lower_str = os_str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    if (lower_str == "windows" || lower_str == "win") return OS::WINDOWS;
    if (lower_str == "linux") return OS::LINUX;
    if (lower_str == "unix") return OS::UNIX;
    if (lower_str == "apple" || lower_str == "macos" || lower_str == "darwin") return OS::APPLE;
    return OS::UNKNOWN;
}

Architecture ArchitectureDetector::string_to_architecture(const std::string& arch_str) {
    std::string lower_str = arch_str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    if (lower_str == "x64" || lower_str == "x86_64" || lower_str == "amd64") return Architecture::X64;
    if (lower_str == "x86" || lower_str == "i386" || lower_str == "i686") return Architecture::X86;
    if (lower_str == "arm64" || lower_str == "aarch64") return Architecture::ARM64;
    if (lower_str == "arm") return Architecture::ARM;
    return Architecture::UNKNOWN;
}

std::vector<std::string> ArchitectureDetector::get_platform_compiler_flags(const Platform& platform) {
    std::vector<std::string> flags;
    
    // Architecture-specific flags
    switch (platform.architecture) {
        case Architecture::X86:
            flags.push_back("-m32");
            break;
        case Architecture::X64:
            flags.push_back("-m64");
            break;
        case Architecture::ARM:
            // ARM-specific flags would go here
            break;
        case Architecture::ARM64:
            // ARM64-specific flags would go here
            break;
        case Architecture::UNKNOWN:
            // No specific flags
            break;
    }
    
    // OS-specific flags
    switch (platform.operating_system) {
        case OS::WINDOWS:
            // Windows-specific flags
            flags.push_back("-static-libgcc");
            flags.push_back("-static-libstdc++");
            break;
        case OS::LINUX:
            // Linux-specific flags
            flags.push_back("-pthread");
            break;
        case OS::UNIX:
            // Generic Unix flags
            flags.push_back("-pthread");
            break;
        case OS::APPLE:
            // macOS-specific flags
            break;
        case OS::UNKNOWN:
            // No specific flags
            break;
    }
    
    return flags;
}

std::string ArchitectureDetector::get_platform_executable_extension(const Platform& platform) {
    switch (platform.operating_system) {
        case OS::WINDOWS:
            return ".exe";
        default:
            return "";
    }
}

std::string ArchitectureDetector::get_platform_shared_library_extension(const Platform& platform) {
    switch (platform.operating_system) {
        case OS::WINDOWS:
            return ".dll";
        case OS::APPLE:
            return ".dylib";
        default:
            return ".so";
    }
}

std::string ArchitectureDetector::get_platform_static_library_extension(const Platform& platform) {
    switch (platform.operating_system) {
        case OS::WINDOWS:
            return ".lib";
        default:
            return ".a";
    }
}

bool Platform::operator<(const Platform& other) const {
    if (operating_system != other.operating_system) {
        return operating_system < other.operating_system;
    }
    return architecture < other.architecture;
}

bool Platform::operator==(const Platform& other) const {
    return operating_system == other.operating_system && architecture == other.architecture;
}

bool Platform::operator!=(const Platform& other) const {
    return !(*this == other);
}