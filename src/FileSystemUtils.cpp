#include "FileSystemUtils.h"
#include "StringUtils.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include <map>
#include <queue>

#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif

std::vector<std::string> FileSystemUtils::expand_source_patterns(const std::vector<std::string>& patterns) {
    std::vector<std::string> result;
    std::set<std::string> unique_files; // Prevent duplicates
    
    for (const std::string& pattern : patterns) {
        std::string trimmed_pattern = StringUtils::trim(pattern);
        
        // Check if it's a direct file reference (no wildcards)
        if (trimmed_pattern.find('*') == std::string::npos) {
            if (file_exists(trimmed_pattern)) {
                unique_files.insert(normalize_path(trimmed_pattern));
            }
            continue;
        }
        
        // Handle wildcard patterns
        size_t slash_pos = trimmed_pattern.find_last_of("/\\");
        std::string directory = ".";
        std::string file_pattern = trimmed_pattern;
        
        if (slash_pos != std::string::npos) {
            directory = trimmed_pattern.substr(0, slash_pos);
            file_pattern = trimmed_pattern.substr(slash_pos + 1);
        }
        
        // Check for recursive pattern (**)
        if (file_pattern == "**" || file_pattern.find("**") != std::string::npos) {
            std::vector<std::string> found_files = find_cpp_files(directory, true);
            for (const std::string& file : found_files) {
                unique_files.insert(normalize_path(file));
            }
        } else {
            // Single directory pattern
            std::vector<std::string> found_files = find_cpp_files(directory, false);
            for (const std::string& file : found_files) {
                if (match_pattern(std::filesystem::path(file).filename().string(), file_pattern)) {
                    unique_files.insert(normalize_path(file));
                }
            }
        }
    }
    
    // Convert set back to vector
    result.assign(unique_files.begin(), unique_files.end());
    
    // Analyze dependencies and sort files in build order
    if (result.size() > 1) {
        result = analyze_dependencies(result);
    }
    
    return result;
}

std::vector<std::string> FileSystemUtils::find_cpp_files(const std::string& directory, bool recursive) {
    std::vector<std::string> files;
    
    if (!directory_exists(directory)) {
        std::cerr << "[WARNING] Directory does not exist: " << directory << std::endl;
        return files;
    }
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file() && is_cpp_source_file(entry.path().string())) {
                    files.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && is_cpp_source_file(entry.path().string())) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[ERROR] Failed to access directory " << directory << ": " << e.what() << std::endl;
    }
    
    return files;
}

bool FileSystemUtils::is_cpp_source_file(const std::string& filename) {
    std::filesystem::path path(filename);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return extension == ".cpp" || extension == ".cxx" || 
           extension == ".cc" || extension == ".c++" || extension == ".c";
}

bool FileSystemUtils::is_cpp_header_file(const std::string& filename) {
    std::filesystem::path path(filename);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return extension == ".h" || extension == ".hpp" || 
           extension == ".hxx" || extension == ".h++";
}

std::vector<std::string> FileSystemUtils::analyze_dependencies(const std::vector<std::string>& source_files) {
    std::map<std::string, std::set<std::string>> dependencies;
    std::map<std::string, std::string> header_to_source;
    
    // First pass: build header to source mapping
    for (const std::string& source_file : source_files) {
        std::filesystem::path source_path(source_file);
        std::string base_name = source_path.stem().string();
        std::string dir = source_path.parent_path().string();
        
        // Look for corresponding header file
        std::vector<std::string> possible_headers = {
            dir + PATH_SEPARATOR + base_name + ".h",
            dir + PATH_SEPARATOR + base_name + ".hpp",
            dir + PATH_SEPARATOR + base_name + ".hxx"
        };
        
        for (const std::string& header : possible_headers) {
            if (file_exists(header)) {
                header_to_source[std::filesystem::path(header).filename().string()] = source_file;
                break;
            }
        }
    }
    
    // Second pass: analyze includes
    for (const std::string& source_file : source_files) {
        dependencies[source_file] = std::set<std::string>();
        std::set<std::string> includes = extract_local_includes(source_file);
        
        for (const std::string& include : includes) {
            if (header_to_source.find(include) != header_to_source.end()) {
                std::string dep_source = header_to_source[include];
                if (dep_source != source_file) {
                    dependencies[source_file].insert(dep_source);
                }
            }
        }
    }
    
    // Perform topological sort
    return topological_sort(dependencies);
}

bool FileSystemUtils::directory_exists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

bool FileSystemUtils::file_exists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

std::string FileSystemUtils::normalize_path(const std::string& path) {
    std::filesystem::path p(path);
    return p.lexically_normal().string();
}

bool FileSystemUtils::match_pattern(const std::string& filename, const std::string& pattern) {
    // Simple wildcard matching (* matches any sequence of characters)
    std::string regex_pattern = pattern;
    
    // Escape special regex characters except *
    std::regex special_chars(R"([\.\^\$\+\(\)\[\]\{\}\|\\])");
    regex_pattern = std::regex_replace(regex_pattern, special_chars, R"(\$&)");
    
    // Replace * with regex equivalent
    std::regex star(R"(\*)");
    regex_pattern = std::regex_replace(regex_pattern, star, ".*");
    
    std::regex matcher(regex_pattern, std::regex_constants::icase);
    return std::regex_match(filename, matcher);
}

std::set<std::string> FileSystemUtils::extract_local_includes(const std::string& filepath) {
    std::set<std::string> includes;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return includes;
    }
    
    std::string line;
    std::regex include_regex("^\\s*#\\s*include\\s*\"([^\"]+)\"\\s*");
    
    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_match(line, match, include_regex)) {
            std::string included_file = match[1].str();
            // Only consider local includes (in quotes, not angle brackets)
            includes.insert(std::filesystem::path(included_file).filename().string());
        }
    }
    
    return includes;
}

std::vector<std::string> FileSystemUtils::topological_sort(const std::map<std::string, std::set<std::string>>& dependencies) {
    std::vector<std::string> result;
    std::map<std::string, int> in_degree;
    std::queue<std::string> zero_in_degree;
    
    // Initialize in-degree count
    for (const auto& pair : dependencies) {
        if (in_degree.find(pair.first) == in_degree.end()) {
            in_degree[pair.first] = 0;
        }
        for (const std::string& dep : pair.second) {
            in_degree[dep]++;
        }
    }
    
    // Find all nodes with in-degree 0
    for (const auto& pair : in_degree) {
        if (pair.second == 0) {
            zero_in_degree.push(pair.first);
        }
    }
    
    // Process nodes
    while (!zero_in_degree.empty()) {
        std::string current = zero_in_degree.front();
        zero_in_degree.pop();
        result.push_back(current);
        
        // Reduce in-degree for dependent nodes
        if (dependencies.find(current) != dependencies.end()) {
            for (const std::string& dependent : dependencies.at(current)) {
                in_degree[dependent]--;
                if (in_degree[dependent] == 0) {
                    zero_in_degree.push(dependent);
                }
            }
        }
    }
    
    // If result size doesn't match input, there's a cycle - return original order
    if (result.size() != dependencies.size()) {
        std::cerr << "[WARNING] Circular dependency detected. Using original file order." << std::endl;
        result.clear();
        for (const auto& pair : dependencies) {
            result.push_back(pair.first);
        }
    }
    
    return result;
}