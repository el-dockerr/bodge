#include "Strings.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

// Static member initialization
Language Strings::current_language_ = Language::ENGLISH;
bool Strings::initialized_ = false;
std::map<Language, std::map<StringID, std::string>> Strings::translations_;

void Strings::initialize() {
    if (initialized_) return;
    
    current_language_ = detect_system_language();
    load_translations();
    initialized_ = true;
}

std::string Strings::get(StringID id) {
    if (!initialized_) {
        initialize();
    }
    
    auto lang_it = translations_.find(current_language_);
    if (lang_it != translations_.end()) {
        auto str_it = lang_it->second.find(id);
        if (str_it != lang_it->second.end()) {
            return str_it->second;
        }
    }
    
    // Fallback to English if translation not found
    auto eng_it = translations_.find(Language::ENGLISH);
    if (eng_it != translations_.end()) {
        auto str_it = eng_it->second.find(id);
        if (str_it != eng_it->second.end()) {
            return str_it->second;
        }
    }
    
    return "[MISSING STRING]";
}

template<typename... Args>
std::string Strings::format(StringID id, Args... args) {
    std::string format_str = get(id);
    // Simple placeholder replacement for now
    // Can be extended with more sophisticated formatting
    return format_str;
}

void Strings::set_language(Language lang) {
    current_language_ = lang;
}

Language Strings::get_language() {
    return current_language_;
}

Language Strings::detect_system_language() {
#ifdef _WIN32
    // Windows: Check system locale
    LCID lcid = GetSystemDefaultLCID();
    LANGID langid = LANGIDFROMLCID(lcid);
    WORD primary = PRIMARYLANGID(langid);
    
    if (primary == LANG_CHINESE) {
        return Language::CHINESE_SIMPLIFIED;
    }
#else
    // Unix/Linux/macOS: Check LANG environment variable
    const char* lang = std::getenv("LANG");
    if (lang != nullptr) {
        std::string lang_str(lang);
        std::transform(lang_str.begin(), lang_str.end(), lang_str.begin(), ::tolower);
        
        if (lang_str.find("zh_cn") != std::string::npos ||
            lang_str.find("zh_hans") != std::string::npos ||
            lang_str.find("chinese") != std::string::npos) {
            return Language::CHINESE_SIMPLIFIED;
        }
    }
#endif
    
    return Language::ENGLISH; // Default
}

void Strings::load_translations() {
    auto& en = translations_[Language::ENGLISH];
    auto& zh = translations_[Language::CHINESE_SIMPLIFIED];
    
    // Application info
    en[StringID::APP_TITLE] = "Bodge - The Idiotic Build System";
    zh[StringID::APP_TITLE] = "Bodge - 简易构建系统";
    
    en[StringID::APP_SUBTITLE] = "Making builds beautiful since 2025";
    zh[StringID::APP_SUBTITLE] = "自2025年起让构建更美好";
    
    en[StringID::APP_AUTHOR] = "Author: Swen \"El Dockerr\" Kalski";
    zh[StringID::APP_AUTHOR] = "作者：Swen \"El Dockerr\" Kalski";
    
    en[StringID::APP_VERSION] = "Version: ";
    zh[StringID::APP_VERSION] = "版本：";
    
    // Commands
    en[StringID::CMD_USAGE] = "Usage: bodge [command] [target/sequence] [options]";
    zh[StringID::CMD_USAGE] = "用法：bodge [命令] [目标/序列] [选项]";
    
    en[StringID::CMD_COMMANDS] = "Commands:";
    zh[StringID::CMD_COMMANDS] = "命令：";
    
    en[StringID::CMD_BUILD] = "  build [target]     - Build specific target (default: all targets)";
    zh[StringID::CMD_BUILD] = "  build [目标]       - 构建指定目标（默认：所有目标）";
    
    en[StringID::CMD_FETCH] = "  fetch              - Fetch git dependencies only";
    zh[StringID::CMD_FETCH] = "  fetch              - 仅获取git依赖项";
    
    en[StringID::CMD_SEQUENCE] = "  sequence [name]    - Execute specific sequence";
    zh[StringID::CMD_SEQUENCE] = "  sequence [名称]    - 执行指定序列";
    
    en[StringID::CMD_WATCH] = "  watch              - Watch mode: automatically rebuild on file changes";
    zh[StringID::CMD_WATCH] = "  watch              - 监视模式：文件更改时自动重新构建";
    
    en[StringID::CMD_DAEMON] = "  daemon             - Alias for watch mode";
    zh[StringID::CMD_DAEMON] = "  daemon             - watch模式的别名";
    
    en[StringID::CMD_LIST] = "  list               - List available targets and sequences";
    zh[StringID::CMD_LIST] = "  list               - 列出可用的目标和序列";
    
    en[StringID::CMD_PLATFORM] = "  platform           - Show current platform information";
    zh[StringID::CMD_PLATFORM] = "  platform           - 显示当前平台信息";
    
    en[StringID::CMD_HELP] = "  help               - Show this help message";
    zh[StringID::CMD_HELP] = "  help               - 显示此帮助信息";
    
    en[StringID::CMD_VERSION] = "  version            - Show version information";
    zh[StringID::CMD_VERSION] = "  version            - 显示版本信息";
    
    // Options
    en[StringID::OPT_OPTIONS] = "Options:";
    zh[StringID::OPT_OPTIONS] = "选项：";
    
    en[StringID::OPT_PLATFORM] = "  --platform=<platform>  - Build for specific platform";
    zh[StringID::OPT_PLATFORM] = "  --platform=<平台>      - 为特定平台构建";
    
    en[StringID::OPT_ARCH] = "  --arch=<arch>          - Build for specific architecture";
    zh[StringID::OPT_ARCH] = "  --arch=<架构>          - 为特定架构构建";
    
    en[StringID::OPT_INTERVAL] = "  --interval=<ms>        - Poll interval for watch mode (default: 1000ms)";
    zh[StringID::OPT_INTERVAL] = "  --interval=<毫秒>      - 监视模式的轮询间隔（默认：1000毫秒）";
    
    en[StringID::OPT_LOG] = "  --log=<file>           - Log file for watch mode (default: bodge_daemon.log)";
    zh[StringID::OPT_LOG] = "  --log=<文件>           - 监视模式的日志文件（默认：bodge_daemon.log）";
    
    // Examples header
    en[StringID::EXAMPLES_HEADER] = "Examples:";
    zh[StringID::EXAMPLES_HEADER] = "示例：";
    
    // Status messages
    en[StringID::MSG_TARGET_PLATFORM] = "Target platform: ";
    zh[StringID::MSG_TARGET_PLATFORM] = "目标平台：";
    
    en[StringID::MSG_CURRENT_PLATFORM_INFO] = "Current platform information:";
    zh[StringID::MSG_CURRENT_PLATFORM_INFO] = "当前平台信息：";
    
    en[StringID::MSG_OS] = "  OS: ";
    zh[StringID::MSG_OS] = "  操作系统：";
    
    en[StringID::MSG_ARCHITECTURE] = "  Architecture: ";
    zh[StringID::MSG_ARCHITECTURE] = "  架构：";
    
    en[StringID::MSG_PLATFORM_STRING] = "  Platform string: ";
    zh[StringID::MSG_PLATFORM_STRING] = "  平台字符串：";
    
    en[StringID::MSG_SUPPORTED_PLATFORMS] = "Supported platforms:";
    zh[StringID::MSG_SUPPORTED_PLATFORMS] = "支持的平台：";
    
    // Available items
    en[StringID::AVAILABLE_TARGETS] = "Available Targets";
    zh[StringID::AVAILABLE_TARGETS] = "可用目标";
    
    en[StringID::AVAILABLE_SEQUENCES] = "Available Sequences";
    zh[StringID::AVAILABLE_SEQUENCES] = "可用序列";
    
    en[StringID::DEFAULT_TARGET_PLATFORMS] = "Default Target Platforms";
    zh[StringID::DEFAULT_TARGET_PLATFORMS] = "默认目标平台";
    
    en[StringID::OPERATIONS_COUNT] = " operations";
    zh[StringID::OPERATIONS_COUNT] = " 个操作";
    
    // Build messages
    en[StringID::BUILD_STARTED] = "Build started";
    zh[StringID::BUILD_STARTED] = "构建开始";
    
    en[StringID::BUILD_SUCCESS] = "Build completed successfully!";
    zh[StringID::BUILD_SUCCESS] = "构建成功完成！";
    
    en[StringID::BUILD_FAILED] = "Build failed";
    zh[StringID::BUILD_FAILED] = "构建失败";
    
    en[StringID::BUILD_DURATION] = "Duration: ";
    zh[StringID::BUILD_DURATION] = "耗时：";
    
    en[StringID::BUILD_FOR_PLATFORM] = "Building for platform: ";
    zh[StringID::BUILD_FOR_PLATFORM] = "为平台构建：";
    
    en[StringID::BUILD_SKIPPING_TARGET] = "Skipping target '";
    zh[StringID::BUILD_SKIPPING_TARGET] = "跳过目标 '";
    
    en[StringID::BUILD_TARGET_COMPLETED] = "Target '";
    zh[StringID::BUILD_TARGET_COMPLETED] = "目标 '";
    
    en[StringID::BUILD_COMMAND_EXECUTING] = "Executing build command...";
    zh[StringID::BUILD_COMMAND_EXECUTING] = "执行构建命令...";
    
    en[StringID::BUILD_EXIT_CODE] = "Build failed with exit code: ";
    zh[StringID::BUILD_EXIT_CODE] = "构建失败，退出代码：";
    
    // Dependency messages
    en[StringID::DEPS_FETCHING] = "Fetching Dependencies";
    zh[StringID::DEPS_FETCHING] = "获取依赖项";
    
    en[StringID::DEPS_FETCHING_URL] = "Fetching: ";
    zh[StringID::DEPS_FETCHING_URL] = "获取：";
    
    en[StringID::DEPS_FETCH_SUCCESS] = "Fetched ";
    zh[StringID::DEPS_FETCH_SUCCESS] = "已获取 ";
    
    en[StringID::DEPS_FETCH_FAILED] = "Failed to fetch ";
    zh[StringID::DEPS_FETCH_FAILED] = "获取失败 ";
    
    en[StringID::DEPS_ALL_FETCHED] = "All dependencies fetched successfully!";
    zh[StringID::DEPS_ALL_FETCHED] = "所有依赖项成功获取！";
    
    en[StringID::DEPS_POST_CLONE_RUNNING] = "Running post-clone command in ";
    zh[StringID::DEPS_POST_CLONE_RUNNING] = "在以下位置运行克隆后命令：";
    
    en[StringID::DEPS_POST_CLONE_SUCCESS] = "Post-clone bodge command completed.";
    zh[StringID::DEPS_POST_CLONE_SUCCESS] = "克隆后的bodge命令已完成。";
    
    en[StringID::DEPS_POST_CLONE_FAILED] = "Post-clone bodge command failed with exit code: ";
    zh[StringID::DEPS_POST_CLONE_FAILED] = "克隆后的bodge命令失败，退出代码：";
    
    // Sequence messages
    en[StringID::SEQ_EXECUTING] = "Executing Sequence: ";
    zh[StringID::SEQ_EXECUTING] = "执行序列：";
    
    en[StringID::SEQ_COMPLETED] = "Sequence '";
    zh[StringID::SEQ_COMPLETED] = "序列 '";
    
    en[StringID::SEQ_FAILED] = "Sequence '";
    zh[StringID::SEQ_FAILED] = "序列 '";
    
    en[StringID::SEQ_PROGRESS] = "Sequence progress";
    zh[StringID::SEQ_PROGRESS] = "序列进度";
    
    // File operations
    en[StringID::FILE_COPYING] = "Copying ";
    zh[StringID::FILE_COPYING] = "复制 ";
    
    en[StringID::FILE_COPY_COMPLETED] = "Copy completed.";
    zh[StringID::FILE_COPY_COMPLETED] = "复制完成。";
    
    en[StringID::FILE_COPY_FAILED] = "Copy failed: ";
    zh[StringID::FILE_COPY_FAILED] = "复制失败：";
    
    en[StringID::FILE_REMOVING] = "Removing ";
    zh[StringID::FILE_REMOVING] = "删除 ";
    
    en[StringID::FILE_REMOVED] = "Removed ";
    zh[StringID::FILE_REMOVED] = "已删除 ";
    
    en[StringID::FILE_REMOVE_FAILED] = "Remove failed: ";
    zh[StringID::FILE_REMOVE_FAILED] = "删除失败：";
    
    en[StringID::FILE_MKDIR] = "Creating directory ";
    zh[StringID::FILE_MKDIR] = "创建目录 ";
    
    en[StringID::FILE_MKDIR_COMPLETED] = "Directory created.";
    zh[StringID::FILE_MKDIR_COMPLETED] = "目录已创建。";
    
    en[StringID::FILE_MKDIR_FAILED] = "Directory creation failed: ";
    zh[StringID::FILE_MKDIR_FAILED] = "目录创建失败：";
    
    en[StringID::FILE_MKDIR_EXISTS] = "Directory already exists.";
    zh[StringID::FILE_MKDIR_EXISTS] = "目录已存在。";
    
    // Daemon mode
    en[StringID::DAEMON_MODE_TITLE] = "--- Bodge Daemon Mode ---";
    zh[StringID::DAEMON_MODE_TITLE] = "--- Bodge 守护进程模式 ---";
    
    en[StringID::DAEMON_LOG_FILE] = "[INFO] Build logs will be written to: ";
    zh[StringID::DAEMON_LOG_FILE] = "[信息] 构建日志将写入：";
    
    en[StringID::DAEMON_STARTED] = "Daemon mode started";
    zh[StringID::DAEMON_STARTED] = "守护进程模式已启动";
    
    en[StringID::DAEMON_WATCHING_FILES] = "[INFO] Watching ";
    zh[StringID::DAEMON_WATCHING_FILES] = "[信息] 监视 ";
    
    en[StringID::DAEMON_POLL_INTERVAL] = "[INFO] Poll interval: ";
    zh[StringID::DAEMON_POLL_INTERVAL] = "[信息] 轮询间隔：";
    
    en[StringID::DAEMON_PRESS_CTRL_C] = "[INFO] Press Ctrl+C to stop daemon mode.";
    zh[StringID::DAEMON_PRESS_CTRL_C] = "[信息] 按Ctrl+C停止守护进程模式。";
    
    en[StringID::DAEMON_INITIAL_BUILD] = "[INFO] Performing initial build...";
    zh[StringID::DAEMON_INITIAL_BUILD] = "[信息] 执行初始构建...";
    
    en[StringID::DAEMON_WATCHING] = "[INFO] Daemon is now watching for file changes...";
    zh[StringID::DAEMON_WATCHING] = "[信息] 守护进程正在监视文件更改...";
    
    en[StringID::DAEMON_CHANGE_DETECTED] = "[CHANGE DETECTED] ";
    zh[StringID::DAEMON_CHANGE_DETECTED] = "[检测到更改] ";
    
    en[StringID::DAEMON_FILES_CHANGED] = " file(s) changed:";
    zh[StringID::DAEMON_FILES_CHANGED] = " 个文件已更改：";
    
    en[StringID::DAEMON_REBUILDING] = "[INFO] Triggering rebuild...";
    zh[StringID::DAEMON_REBUILDING] = "[信息] 触发重新构建...";
    
    en[StringID::DAEMON_REBUILD_SUCCESS] = "[SUCCESS] Rebuild completed successfully.";
    zh[StringID::DAEMON_REBUILD_SUCCESS] = "[成功] 重新构建成功完成。";
    
    en[StringID::DAEMON_REBUILD_FAILED] = "[ERROR] Rebuild failed. See log for details.";
    zh[StringID::DAEMON_REBUILD_FAILED] = "[错误] 重新构建失败。详见日志。";
    
    en[StringID::DAEMON_STOPPED] = "[INFO] Daemon mode stopped.";
    zh[StringID::DAEMON_STOPPED] = "[信息] 守护进程模式已停止。";
    
    // Errors
    en[StringID::ERR_CONFIG_INVALID] = "[ERROR] Configuration is invalid. Please check your .bodge file.";
    zh[StringID::ERR_CONFIG_INVALID] = "[错误] 配置无效。请检查您的.bodge文件。";
    
    en[StringID::ERR_CONFIG_INCOMPLETE] = "[FATAL] Configuration is critically incomplete. Please ensure required fields are set in .bodge.";
    zh[StringID::ERR_CONFIG_INCOMPLETE] = "[严重] 配置严重不完整。请确保在.bodge中设置了必需的字段。";
    
    en[StringID::ERR_TARGET_NOT_FOUND] = "[ERROR] Target '";
    zh[StringID::ERR_TARGET_NOT_FOUND] = "[错误] 未找到目标 '";
    
    en[StringID::ERR_SEQUENCE_NOT_FOUND] = "[ERROR] Sequence '";
    zh[StringID::ERR_SEQUENCE_NOT_FOUND] = "[错误] 未找到序列 '";
    
    en[StringID::ERR_SPECIFY_SEQUENCE] = "[ERROR] Please specify a sequence name.";
    zh[StringID::ERR_SPECIFY_SEQUENCE] = "[错误] 请指定序列名称。";
    
    en[StringID::ERR_UNKNOWN_COMMAND] = "[ERROR] Unknown command: ";
    zh[StringID::ERR_UNKNOWN_COMMAND] = "[错误] 未知命令：";
    
    en[StringID::ERR_USE_HELP] = "Use 'bodge help' for usage information.";
    zh[StringID::ERR_USE_HELP] = "使用'bodge help'获取使用信息。";
    
    en[StringID::ERR_GIT_NOT_AVAILABLE] = "[ERROR] Git is not available. Please install Git and ensure it is in your system's PATH.";
    zh[StringID::ERR_GIT_NOT_AVAILABLE] = "[错误] Git不可用。请安装Git并确保它在系统的PATH中。";
    
    en[StringID::ERR_GIT_INVALID_URL] = "[ERROR] Invalid git repository URL: ";
    zh[StringID::ERR_GIT_INVALID_URL] = "[错误] 无效的git仓库URL：";
    
    en[StringID::ERR_GIT_INVALID_PATH] = "[ERROR] Invalid local path: ";
    zh[StringID::ERR_GIT_INVALID_PATH] = "[错误] 无效的本地路径：";
    
    en[StringID::ERR_GIT_CLONE_FAILED] = "[ERROR] Git clone failed.";
    zh[StringID::ERR_GIT_CLONE_FAILED] = "[错误] Git克隆失败。";
    
    en[StringID::ERR_GIT_PULL_FAILED] = "[WARNING] Pull failed. Directory exists but may not be a valid git repository.";
    zh[StringID::ERR_GIT_PULL_FAILED] = "[警告] 拉取失败。目录存在但可能不是有效的git仓库。";
    
    en[StringID::ERR_DIR_CHANGE_FAILED] = "[ERROR] Failed to change directory: ";
    zh[StringID::ERR_DIR_CHANGE_FAILED] = "[错误] 更改目录失败：";
    
    en[StringID::ERR_DIR_RESTORE_FAILED] = "[ERROR] Failed to restore original directory!";
    zh[StringID::ERR_DIR_RESTORE_FAILED] = "[错误] 恢复原始目录失败！";
    
    en[StringID::ERR_DEPS_MISMATCH] = "[ERROR] Mismatch between number of dependency git URLs and git paths.";
    zh[StringID::ERR_DEPS_MISMATCH] = "[错误] 依赖项git URL数量与git路径数量不匹配。";
    
    en[StringID::ERR_INVALID_COMPILER] = "[ERROR] Invalid compiler specified";
    zh[StringID::ERR_INVALID_COMPILER] = "[错误] 指定的编译器无效";
    
    en[StringID::ERR_INVALID_ARGUMENTS] = "[ERROR] Invalid compiler arguments detected";
    zh[StringID::ERR_INVALID_ARGUMENTS] = "[错误] 检测到无效的编译器参数";
    
    en[StringID::ERR_COMMAND_EMPTY] = "[ERROR] Build command is empty or invalid";
    zh[StringID::ERR_COMMAND_EMPTY] = "[错误] 构建命令为空或无效";
    
    en[StringID::ERR_COMMAND_TOO_LONG] = "[ERROR] Build command is too long";
    zh[StringID::ERR_COMMAND_TOO_LONG] = "[错误] 构建命令过长";
    
    en[StringID::ERR_INDEX_OUT_OF_BOUNDS] = "[ERROR] Index out of bounds in dependencies arrays.";
    zh[StringID::ERR_INDEX_OUT_OF_BOUNDS] = "[错误] 依赖项数组索引越界。";
    
    en[StringID::ERR_FILE_WATCHER_INIT] = "[ERROR] Failed to initialize file watcher.";
    zh[StringID::ERR_FILE_WATCHER_INIT] = "[错误] 初始化文件监视器失败。";
    
    en[StringID::ERR_LOG_FILE_OPEN] = "[ERROR] Failed to open log file: ";
    zh[StringID::ERR_LOG_FILE_OPEN] = "[错误] 打开日志文件失败：";
    
    en[StringID::ERR_GET_CURRENT_DIR] = "[ERROR] Failed to get current directory: ";
    zh[StringID::ERR_GET_CURRENT_DIR] = "[错误] 获取当前目录失败：";
    
    // Warnings
    en[StringID::WARN_EMPTY_PLATFORM] = "[WARNING] Empty platform value, using current platform";
    zh[StringID::WARN_EMPTY_PLATFORM] = "[警告] 平台值为空，使用当前平台";
    
    en[StringID::WARN_EMPTY_ARCH] = "[WARNING] Empty architecture value, ignoring";
    zh[StringID::WARN_EMPTY_ARCH] = "[警告] 架构值为空，忽略";
    
    en[StringID::WARN_UNKNOWN_ARCH] = "[WARNING] Unknown architecture: ";
    zh[StringID::WARN_UNKNOWN_ARCH] = "[警告] 未知架构：";
    
    en[StringID::WARN_EMPTY_INTERVAL] = "[WARNING] Empty interval value, using default (1000ms)";
    zh[StringID::WARN_EMPTY_INTERVAL] = "[警告] 间隔值为空，使用默认值（1000毫秒）";
    
    en[StringID::WARN_INVALID_INTERVAL] = "[WARNING] Invalid interval value, using default (1000ms)";
    zh[StringID::WARN_INVALID_INTERVAL] = "[警告] 间隔值无效，使用默认值（1000毫秒）";
    
    en[StringID::WARN_INTERVAL_OUT_OF_RANGE] = "[WARNING] Interval out of range (100-3600000ms), using default (1000ms)";
    zh[StringID::WARN_INTERVAL_OUT_OF_RANGE] = "[警告] 间隔超出范围（100-3600000毫秒），使用默认值（1000毫秒）";
    
    en[StringID::WARN_EMPTY_LOG_FILE] = "[WARNING] Empty log file path, using default (bodge_daemon.log)";
    zh[StringID::WARN_EMPTY_LOG_FILE] = "[警告] 日志文件路径为空，使用默认值（bodge_daemon.log）";
    
    en[StringID::WARN_INVALID_LOG_FILE] = "[WARNING] Invalid log file path, using default (bodge_daemon.log)";
    zh[StringID::WARN_INVALID_LOG_FILE] = "[警告] 日志文件路径无效，使用默认值（bodge_daemon.log）";
    
    en[StringID::WARN_UNKNOWN_OPTION] = "[WARNING] Unknown option: ";
    zh[StringID::WARN_UNKNOWN_OPTION] = "[警告] 未知选项：";
    
    en[StringID::WARN_SYSTEM_NOT_SUPPORTED] = "[WARNING] System command execution is not fully supported on this platform. Build may fail.";
    zh[StringID::WARN_SYSTEM_NOT_SUPPORTED] = "[警告] 此平台不完全支持系统命令执行。构建可能会失败。";
    
    en[StringID::WARN_GIT_PULL_WARNING] = "[WARNING] Pull failed. Directory exists but may not be a valid git repository.";
    zh[StringID::WARN_GIT_PULL_WARNING] = "[警告] 拉取失败。目录存在但可能不是有效的git仓库。";
    
    en[StringID::WARN_DIR_NOT_EXIST] = "[WARNING] Directory does not exist: ";
    zh[StringID::WARN_DIR_NOT_EXIST] = "[警告] 目录不存在：";
    
    en[StringID::WARN_CIRCULAR_DEPENDENCY] = "[WARNING] Circular dependency detected. Using original file order.";
    zh[StringID::WARN_CIRCULAR_DEPENDENCY] = "[警告] 检测到循环依赖。使用原始文件顺序。";
    
    // Info messages
    en[StringID::INFO_REPO_CLONED] = "[SUCCESS] Repository already cloned. Successfully pulled latest changes.";
    zh[StringID::INFO_REPO_CLONED] = "[成功] 仓库已克隆。成功拉取最新更改。";
    
    en[StringID::INFO_REPO_PULLED] = "[SUCCESS] Successfully pulled latest changes.";
    zh[StringID::INFO_REPO_PULLED] = "[成功] 成功拉取最新更改。";
    
    en[StringID::INFO_REPO_CLONING] = "[INFO] Repository not found locally. Attempting to clone...";
    zh[StringID::INFO_REPO_CLONING] = "[信息] 本地未找到仓库。尝试克隆...";
    
    en[StringID::INFO_NOTHING_TO_REMOVE] = "[INFO] Nothing to remove (path doesn't exist).";
    zh[StringID::INFO_NOTHING_TO_REMOVE] = "[信息] 没有可删除的内容（路径不存在）。";
    
    en[StringID::INFO_DIR_ALREADY_EXISTS] = "[INFO] Directory already exists.";
    zh[StringID::INFO_DIR_ALREADY_EXISTS] = "[信息] 目录已存在。";
    
    en[StringID::INFO_OVERALL_PROGRESS] = "Overall progress (";
    zh[StringID::INFO_OVERALL_PROGRESS] = "总体进度（";
    
    // Git success messages
    en[StringID::GIT_REPO_CLONED] = "[SUCCESS] Successfully cloned repository.";
    zh[StringID::GIT_REPO_CLONED] = "[成功] 成功克隆仓库。";
    
    en[StringID::GIT_REPO_PULLED] = "[SUCCESS] Repository already cloned. Successfully pulled latest changes.";
    zh[StringID::GIT_REPO_PULLED] = "[成功] 仓库已克隆。成功拉取最新更改。";
    
    en[StringID::GIT_REPO_CLONING] = "[INFO] Repository not found locally. Attempting to clone...";
    zh[StringID::GIT_REPO_CLONING] = "[信息] 本地未找到仓库。尝试克隆...";
}

