#include "ProgressBar.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

int ProgressBar::spinner_state_ = 0;
static bool console_initialized = false;

// Initialize Windows console for UTF-8 and ANSI colors
static void initialize_console() {
    if (console_initialized) return;
    console_initialized = true;
    
#ifdef _WIN32
    // Set console output to UTF-8
    SetConsoleOutputCP(CP_UTF8);
    
    // Enable ANSI color support on Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
#endif
}

ProgressBar::ProgressBar(int total, int width)
    : total_(total), width_(width) {
    initialize_console();
}

bool ProgressBar::supports_ansi() {
    // Check if stdout is a terminal
    static bool is_tty = ISATTY(FILENO(stdout)) != 0;
    return is_tty;
}

std::string ProgressBar::get_color(const std::string& color) {
    if (!supports_ansi()) return "";
    
    if (color == "red") return "\033[31m";
    if (color == "green") return "\033[32m";
    if (color == "yellow") return "\033[33m";
    if (color == "blue") return "\033[34m";
    if (color == "magenta") return "\033[35m";
    if (color == "cyan") return "\033[36m";
    if (color == "white") return "\033[37m";
    if (color == "bright_green") return "\033[92m";
    if (color == "bright_yellow") return "\033[93m";
    if (color == "bright_blue") return "\033[94m";
    if (color == "bright_magenta") return "\033[95m";
    if (color == "bright_cyan") return "\033[96m";
    if (color == "bright_white") return "\033[97m";
    if (color == "bold") return "\033[1m";
    if (color == "dim") return "\033[2m";
    
    return "";
}

std::string ProgressBar::reset_color() {
    if (!supports_ansi()) return "";
    return "\033[0m";
}

void ProgressBar::clear_line() {
    if (supports_ansi()) {
        std::cout << "\033[2K";
    }
}

void ProgressBar::move_cursor_to_start() {
    if (supports_ansi()) {
        std::cout << "\r";
    }
}

void ProgressBar::display(int current, const std::string& prefix) {
    if (current > total_) current = total_;
    
    float progress = static_cast<float>(current) / static_cast<float>(total_);
    int filled = static_cast<int>(progress * width_);
    
    move_cursor_to_start();
    clear_line();
    
    std::cout << get_color("cyan") << prefix << reset_color() << " [";
    
    // Draw filled portion with gradient
    for (int i = 0; i < width_; ++i) {
        if (i < filled) {
            std::cout << get_color("bright_green") << "█" << reset_color();
        } else if (i == filled && filled < width_) {
            std::cout << get_color("green") << "▓" << reset_color();
        } else {
            std::cout << get_color("dim") << "░" << reset_color();
        }
    }
    
    std::cout << "] " << get_color("bright_white") 
              << std::fixed << std::setprecision(1) << (progress * 100.0f) << "%" 
              << reset_color();
    
    if (current >= total_) {
        std::cout << " " << get_color("bright_green") << "✓" << reset_color() << std::endl;
    } else {
        std::cout << std::flush;
    }
}

void ProgressBar::display_phase_header(const std::string& phase_name, const std::string& icon) {
    initialize_console();
    std::cout << std::endl;
    std::cout << get_color("bright_cyan") << get_color("bold") 
              << "╔══════════════════════════════════════════════════════════════╗" 
              << reset_color() << std::endl;
    
    std::cout << get_color("bright_cyan") << "║ " << reset_color()
              << get_color("bright_magenta") << icon << " " << reset_color()
              << get_color("bright_white") << get_color("bold")
              << std::left << std::setw(56) << phase_name
              << reset_color() << get_color("bright_cyan") << " ║" << reset_color() << std::endl;
    
    std::cout << get_color("bright_cyan") << get_color("bold")
              << "╚══════════════════════════════════════════════════════════════╝"
              << reset_color() << std::endl;
}

void ProgressBar::display_success(const std::string& message) {
    std::cout << get_color("bright_green") << "✓ " << reset_color()
              << get_color("green") << "[SUCCESS] " << reset_color()
              << message << std::endl;
}

void ProgressBar::display_error(const std::string& message) {
    std::cout << get_color("red") << "✗ " << reset_color()
              << get_color("red") << "[ERROR] " << reset_color()
              << message << std::endl;
}

void ProgressBar::display_info(const std::string& message) {
    std::cout << get_color("bright_cyan") << "ℹ " << reset_color()
              << get_color("cyan") << "[INFO] " << reset_color()
              << message << std::endl;
}

void ProgressBar::display_warning(const std::string& message) {
    std::cout << get_color("bright_yellow") << "⚠ " << reset_color()
              << get_color("yellow") << "[WARNING] " << reset_color()
              << message << std::endl;
}

void ProgressBar::display_build_summary(bool success, long long duration_ms, const std::string& target_name) {
    initialize_console();
    std::cout << std::endl;
    
    std::string border_color = success ? "bright_green" : "red";
    std::string status_color = success ? "bright_green" : "red";
    std::string status_icon = success ? "✓" : "✗";
    std::string status_text = success ? "BUILD SUCCESS" : "BUILD FAILED";
    
    std::cout << get_color(border_color) << get_color("bold")
              << "╔══════════════════════════════════════════════════════════════╗"
              << reset_color() << std::endl;
    
    std::cout << get_color(border_color) << "║ " << reset_color()
              << get_color(status_color) << get_color("bold")
              << status_icon << " " << std::left << std::setw(54) << status_text
              << reset_color() << get_color(border_color) << " ║" << reset_color() << std::endl;
    
    if (!target_name.empty()) {
        std::string target_line = "Target: " + target_name;
        std::cout << get_color(border_color) << "║ " << reset_color()
                  << get_color("bright_white") << std::left << std::setw(58) << target_line
                  << reset_color() << get_color(border_color) << " ║" << reset_color() << std::endl;
    }
    
    double duration_sec = duration_ms / 1000.0;
    std::ostringstream duration_stream;
    duration_stream << "Duration: " << std::fixed << std::setprecision(2) << duration_sec << "s";
    std::string duration_line = duration_stream.str();
    
    std::cout << get_color(border_color) << "║ " << reset_color()
              << get_color("bright_white") << std::left << std::setw(58) << duration_line
              << reset_color() << get_color(border_color) << " ║" << reset_color() << std::endl;
    
    std::cout << get_color(border_color) << get_color("bold")
              << "╚══════════════════════════════════════════════════════════════╝"
              << reset_color() << std::endl;
}

void ProgressBar::display_spinner(const std::string& message) {
    const char* spinner_chars[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    const int num_states = 10;
    
    move_cursor_to_start();
    clear_line();
    
    std::cout << get_color("bright_cyan") 
              << spinner_chars[spinner_state_ % num_states] 
              << reset_color() << " " << message << std::flush;
    
    spinner_state_++;
}

void ProgressBar::display_header() {
    initialize_console();
    std::cout << get_color("bright_magenta") << get_color("bold") << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                                ║" << std::endl;
    std::cout << "║            🔨 BODGE - The Idiotic Build System 🔨             ║" << std::endl;
    std::cout << "║                                                                ║" << std::endl;
    std::cout << "║              Making builds beautiful since 2025                ║" << std::endl;
    std::cout << "║                                                                ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << reset_color() << std::endl;
}

