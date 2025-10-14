#pragma once

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>

/**
 * @brief Utility class for string manipulation operations
 */
class StringUtils {
public:
    /**
     * @brief Splits a string by a delimiter and returns a vector of trimmed strings
     * @param s The string to split (e.g., "item1, item2, item3")
     * @param delimiter The character used for splitting
     * @return A vector of strings
     */
    static std::vector<std::string> split(const std::string& s, char delimiter);

    /**
     * @brief Trims whitespace from the beginning and end of a string
     * @param str The string to trim
     * @return The trimmed string
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Joins a vector of strings with a prefix and delimiter
     * @param items Vector of strings to join
     * @param prefix Prefix to prepend to each item (e.g., "-I")
     * @param delimiter Separator between items (default is space)
     * @return The joined string
     */
    static std::string join(const std::vector<std::string>& items, 
                           const std::string& prefix, 
                           const std::string& delimiter = " ");
};

#endif // STRING_UTILS_H