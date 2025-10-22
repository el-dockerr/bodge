#pragma once

#ifndef STRINGS_H
#define STRINGS_H

#include <string>
#include <map>

/**
 * @brief Language codes supported by the build system
 */
enum class Language {
    ENGLISH,
    CHINESE_SIMPLIFIED
};

/**
 * @brief String identifiers for all user-facing messages
 */
enum class StringID {
    // Application info
    APP_TITLE,
    APP_SUBTITLE,
    APP_VERSION,
    APP_AUTHOR,
    
    // Commands and help
    CMD_USAGE,
    CMD_COMMANDS,
    CMD_BUILD,
    CMD_FETCH,
    CMD_SEQUENCE,
    CMD_WATCH,
    CMD_DAEMON,
    CMD_LIST,
    CMD_PLATFORM,
    CMD_HELP,
    CMD_VERSION,
    
    // Options
    OPT_OPTIONS,
    OPT_PLATFORM,
    OPT_ARCH,
    OPT_INTERVAL,
    OPT_LOG,
    
    // Examples
    EXAMPLES_HEADER,
    
    // Status messages
    MSG_TARGET_PLATFORM,
    MSG_CURRENT_PLATFORM_INFO,
    MSG_OS,
    MSG_ARCHITECTURE,
    MSG_PLATFORM_STRING,
    MSG_SUPPORTED_PLATFORMS,
    
    // Available items
    AVAILABLE_TARGETS,
    AVAILABLE_SEQUENCES,
    DEFAULT_TARGET_PLATFORMS,
    OPERATIONS_COUNT,
    
    // Build messages
    BUILD_STARTED,
    BUILD_SUCCESS,
    BUILD_FAILED,
    BUILD_DURATION,
    BUILD_FOR_PLATFORM,
    BUILD_SKIPPING_TARGET,
    BUILD_TARGET_COMPLETED,
    BUILD_COMMAND_EXECUTING,
    BUILD_EXIT_CODE,
    
    // Dependency messages
    DEPS_FETCHING,
    DEPS_FETCHING_URL,
    DEPS_FETCH_SUCCESS,
    DEPS_FETCH_FAILED,
    DEPS_ALL_FETCHED,
    DEPS_POST_CLONE_RUNNING,
    DEPS_POST_CLONE_SUCCESS,
    DEPS_POST_CLONE_FAILED,
    
    // Sequence messages
    SEQ_EXECUTING,
    SEQ_COMPLETED,
    SEQ_FAILED,
    SEQ_PROGRESS,
    
    // File operations
    FILE_COPYING,
    FILE_COPY_COMPLETED,
    FILE_COPY_FAILED,
    FILE_REMOVING,
    FILE_REMOVED,
    FILE_REMOVE_FAILED,
    FILE_MKDIR,
    FILE_MKDIR_COMPLETED,
    FILE_MKDIR_FAILED,
    FILE_MKDIR_EXISTS,
    
    // Daemon mode
    DAEMON_MODE_TITLE,
    DAEMON_LOG_FILE,
    DAEMON_STARTED,
    DAEMON_WATCHING_FILES,
    DAEMON_POLL_INTERVAL,
    DAEMON_PRESS_CTRL_C,
    DAEMON_INITIAL_BUILD,
    DAEMON_WATCHING,
    DAEMON_CHANGE_DETECTED,
    DAEMON_FILES_CHANGED,
    DAEMON_REBUILDING,
    DAEMON_REBUILD_SUCCESS,
    DAEMON_REBUILD_FAILED,
    DAEMON_STOPPED,
    
    // Errors
    ERR_CONFIG_INVALID,
    ERR_CONFIG_INCOMPLETE,
    ERR_TARGET_NOT_FOUND,
    ERR_SEQUENCE_NOT_FOUND,
    ERR_SPECIFY_SEQUENCE,
    ERR_UNKNOWN_COMMAND,
    ERR_USE_HELP,
    ERR_GIT_NOT_AVAILABLE,
    ERR_GIT_INVALID_URL,
    ERR_GIT_INVALID_PATH,
    ERR_GIT_CLONE_FAILED,
    ERR_GIT_PULL_FAILED,
    ERR_DIR_CHANGE_FAILED,
    ERR_DIR_RESTORE_FAILED,
    ERR_DEPS_MISMATCH,
    ERR_INVALID_COMPILER,
    ERR_INVALID_ARGUMENTS,
    ERR_COMMAND_EMPTY,
    ERR_COMMAND_TOO_LONG,
    ERR_INDEX_OUT_OF_BOUNDS,
    ERR_FILE_WATCHER_INIT,
    ERR_LOG_FILE_OPEN,
    ERR_GET_CURRENT_DIR,
    
    // Warnings
    WARN_EMPTY_PLATFORM,
    WARN_EMPTY_ARCH,
    WARN_UNKNOWN_ARCH,
    WARN_EMPTY_INTERVAL,
    WARN_INVALID_INTERVAL,
    WARN_INTERVAL_OUT_OF_RANGE,
    WARN_EMPTY_LOG_FILE,
    WARN_INVALID_LOG_FILE,
    WARN_UNKNOWN_OPTION,
    WARN_SYSTEM_NOT_SUPPORTED,
    WARN_GIT_PULL_WARNING,
    WARN_DIR_NOT_EXIST,
    WARN_CIRCULAR_DEPENDENCY,
    
    // Info messages
    INFO_REPO_CLONED,
    INFO_REPO_PULLED,
    INFO_REPO_CLONING,
    INFO_NOTHING_TO_REMOVE,
    INFO_DIR_ALREADY_EXISTS,
    INFO_OVERALL_PROGRESS,
    
    // Git success messages
    GIT_REPO_CLONED,
    GIT_REPO_PULLED,
    GIT_REPO_CLONING
};

/**
 * @brief Internationalization manager for the build system
 * 
 * Provides language detection and translation services for all
 * user-facing strings in the application.
 */
class Strings {
public:
    /**
     * @brief Initializes the string system with system language
     */
    static void initialize();
    
    /**
     * @brief Gets a translated string by ID
     * @param id String identifier
     * @return Translated string in current language
     */
    static std::string get(StringID id);
    
    /**
     * @brief Gets a formatted string with arguments
     * @param id String identifier
     * @param args Variable number of arguments to format
     * @return Formatted translated string
     */
    template<typename... Args>
    static std::string format(StringID id, Args... args);
    
    /**
     * @brief Sets the current language
     * @param lang Language to use
     */
    static void set_language(Language lang);
    
    /**
     * @brief Gets the current language
     * @return Current language setting
     */
    static Language get_language();
    
    /**
     * @brief Detects system language from environment
     * @return Detected language (defaults to English if unknown)
     */
    static Language detect_system_language();

private:
    static Language current_language_;
    static bool initialized_;
    
    /**
     * @brief Loads all translations into memory
     */
    static void load_translations();
    
    /**
     * @brief Storage for all translations
     * Structure: translations_[language][string_id] = translated_string
     */
    static std::map<Language, std::map<StringID, std::string>> translations_;
};

// Helper macros for convenience
#define STR(id) Strings::get(StringID::id)
#define STRF(id, ...) Strings::format(StringID::id, __VA_ARGS__)

#endif // STRINGS_H

