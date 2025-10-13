#include "StringUtils.h"
#include <sstream>
#include <algorithm>

std::vector<std::string> StringUtils::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::string StringUtils::trim(const std::string& str) {
    std::string result = str;
    // Trim leading whitespace and carriage returns
    result.erase(0, result.find_first_not_of(" \t\r\n"));
    // Trim trailing whitespace and carriage returns
    result.erase(result.find_last_not_of(" \t\r\n") + 1);
    return result;
}

std::string StringUtils::join(const std::vector<std::string>& items, 
                             const std::string& prefix, 
                             const std::string& delimiter) {
    std::stringstream ss;
    for (const auto& item : items) {
        ss << prefix << item << delimiter;
    }
    return ss.str();
}