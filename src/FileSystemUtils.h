#pragma once

#ifndef FILESYSTEM_UTILS_H
#define FILESYSTEM_UTILS_H

#include <string>
#include <vector>
#include <set>
#include <map>

/**
 * @brief Utility class for file system operations and automatic source collection
 */
class FileSystemUtils {
public:
    /**
     * @brief Expands source patterns to actual file paths
     * @param patterns Vector of source patterns
     * @return Vector of resolved file paths
     */
    static std::vector<std::string> expand_source_patterns(const std::vector<std::string>& patterns);
    
    /**
     * @brief Recursively finds all C/C++ source files in a directory
     * @param directory The directory to search (e.g., "src")
     * @param recursive Whether to search subdirectories
     * @return Vector of C/C++ source file paths
     */
    static std::vector<std::string> find_cpp_files(const std::string& directory, bool recursive = false);
    
    /**
     * @brief Checks if a file has a C/C++ source extension
     * @param filename The filename to check
     * @return true if it's a C/C++ source file (.c, .cpp, .cxx, .cc, .c++)
     */
    static bool is_cpp_source_file(const std::string& filename);
    
    /**
     * @brief Checks if a file has a C++ header extension
     * @param filename The filename to check
     * @return true if it's a C++ header file (.h, .hpp, .hxx, .h++)
     */
    static bool is_cpp_header_file(const std::string& filename);
    
    /**
     * @brief Analyzes dependencies between C/C++ files to determine build order
     * @param source_files Vector of source file paths
     * @return Vector of source files ordered by dependencies
     */
    static std::vector<std::string> analyze_dependencies(const std::vector<std::string>& source_files);
    
    /**
     * @brief Checks if a directory exists
     * @param path Directory path to check
     * @return true if directory exists
     */
    static bool directory_exists(const std::string& path);
    
    /**
     * @brief Checks if a file exists
     * @param path File path to check
     * @return true if file exists
     */
    static bool file_exists(const std::string& path);
    
    /**
     * @brief Normalizes path separators for current platform
     * @param path Path to normalize
     * @return Normalized path
     */
    static std::string normalize_path(const std::string& path);

private:
    /**
     * @brief Recursively searches directory for files matching pattern
     * @param directory Directory to search
     * @param pattern Pattern to match (supports * and **)
     * @param results Vector to store results
     */
    static void recursive_find(const std::string& directory, const std::string& pattern, std::vector<std::string>& results);
    
    /**
     * @brief Matches a filename against a pattern with wildcards
     * @param filename Filename to check
     * @param pattern Pattern with wildcards (* and **)
     * @return true if filename matches pattern
     */
    static bool match_pattern(const std::string& filename, const std::string& pattern);
    
    /**
     * @brief Extracts #include statements from a C/C++ file
     * @param filepath Path to the C/C++ file
     * @return Set of included header files (local includes only)
     */
    static std::set<std::string> extract_local_includes(const std::string& filepath);
    
    /**
     * @brief Performs topological sort for dependency ordering
     * @param dependencies Map of file -> set of dependencies
     * @return Vector of files in dependency order
     */
    static std::vector<std::string> topological_sort(const std::map<std::string, std::set<std::string>>& dependencies);
};

#endif // FILESYSTEM_UTILS_H