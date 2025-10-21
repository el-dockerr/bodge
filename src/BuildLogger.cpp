#include "BuildLogger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

BuildLogger::BuildLogger(const std::string& log_file_path)
    : log_file_path_(log_file_path), build_count_(0), success_count_(0), failure_count_(0) {
}

BuildLogger::~BuildLogger() {
    if (log_file_.is_open()) {
        // Write summary before closing
        write_log_line("==========================================================");
        write_log_line("Daemon session ended");
        std::stringstream summary;
        summary << "Build summary: " << build_count_ << " total builds, "
                << success_count_ << " successful, " << failure_count_ << " failed";
        write_log_line(summary.str());
        write_log_line("==========================================================");
        close();
    }
}

bool BuildLogger::open() {
    try {
        log_file_.open(log_file_path_, std::ios::out | std::ios::app);
        if (!log_file_.is_open()) {
            std::cerr << "[ERROR] Failed to open log file: " << log_file_path_ << std::endl;
            return false;
        }

        write_log_line("==========================================================");
        write_log_line("Bodge Daemon started");
        write_log_line("==========================================================");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception opening log file: " << e.what() << std::endl;
        return false;
    }
}

void BuildLogger::close() {
    if (log_file_.is_open()) {
        log_file_.flush();
        log_file_.close();
    }
}

void BuildLogger::log_build_start(const std::string& target_name) {
    build_count_++;
    std::stringstream msg;
    msg << "Build #" << build_count_ << " started";
    if (!target_name.empty()) {
        msg << " - Target: " << target_name;
    }
    write_log_line(msg.str());
}

void BuildLogger::log_build_end(E_RESULT result, long long duration_ms) {
    std::stringstream msg;
    msg << "Build #" << build_count_ << " ";
    
    if (result == S_OK) {
        msg << "SUCCEEDED";
        success_count_++;
    } else {
        msg << "FAILED (error code: " << result << ")";
        failure_count_++;
    }
    
    msg << " - Duration: " << duration_ms << " ms";
    write_log_line(msg.str());
    
    // Write separator after each build
    write_log_line("----------------------------------------------------------");
}

void BuildLogger::log_message(const std::string& message) {
    write_log_line("[INFO] " + message);
}

void BuildLogger::log_error(const std::string& error) {
    write_log_line("[ERROR] " + error);
}

void BuildLogger::log_changed_files(const std::vector<std::string>& changed_files) {
    write_log_line("File changes detected:");
    for (const std::string& file : changed_files) {
        write_log_line("  - " + file);
    }
}

bool BuildLogger::is_open() const {
    return log_file_.is_open();
}

std::string BuildLogger::get_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &now_time_t);
#else
    localtime_r(&now_time_t, &tm_buf);
#endif
    
    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    return ss.str();
}

void BuildLogger::write_log_line(const std::string& message) {
    if (log_file_.is_open()) {
        log_file_ << "[" << get_timestamp() << "] " << message << std::endl;
        log_file_.flush(); // Ensure immediate write
    }
}

