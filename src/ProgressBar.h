#pragma once

#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <string>
#include <iostream>
#include <chrono>

/**
 * @brief Class for displaying visual progress bars and build phase indicators
 */
class ProgressBar {
public:
    /**
     * @brief Constructor
     * @param total Total number of steps
     * @param width Width of the progress bar in characters (default: 50)
     */
    explicit ProgressBar(int total = 100, int width = 50);

    /**
     * @brief Displays a progress bar with current progress
     * @param current Current step
     * @param prefix Text to display before the progress bar
     */
    void display(int current, const std::string& prefix = "");

    /**
     * @brief Displays a phase header with fancy formatting
     * @param phase_name Name of the current phase
     * @param icon Icon character to display
     */
    static void display_phase_header(const std::string& phase_name, const std::string& icon = "►");

    /**
     * @brief Displays a success message with formatting
     * @param message Success message
     */
    static void display_success(const std::string& message);

    /**
     * @brief Displays an error message with formatting
     * @param message Error message
     */
    static void display_error(const std::string& message);

    /**
     * @brief Displays an info message with formatting
     * @param message Info message
     */
    static void display_info(const std::string& message);

    /**
     * @brief Displays a warning message with formatting
     * @param message Warning message
     */
    static void display_warning(const std::string& message);

    /**
     * @brief Displays a build summary box
     * @param success Whether build was successful
     * @param duration_ms Build duration in milliseconds
     * @param target_name Name of the target built
     */
    static void display_build_summary(bool success, long long duration_ms, const std::string& target_name = "");

    /**
     * @brief Displays a spinner animation (call repeatedly)
     * @param message Message to display with spinner
     */
    static void display_spinner(const std::string& message);

    /**
     * @brief Clears the current line
     */
    static void clear_line();

    /**
     * @brief Moves cursor to beginning of line
     */
    static void move_cursor_to_start();

    /**
     * @brief Displays a fancy header for the build system
     */
    static void display_header();

private:
    int total_;
    int width_;
    static int spinner_state_;

    /**
     * @brief Returns ANSI color code
     */
    static std::string get_color(const std::string& color);

    /**
     * @brief Returns ANSI reset code
     */
    static std::string reset_color();

    /**
     * @brief Checks if terminal supports ANSI colors
     */
    static bool supports_ansi();
};

#endif // PROGRESS_BAR_H

