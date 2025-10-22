---
layout: default
title: Daemon Mode
---

# Daemon Mode - Automatic Rebuilds

Bodge includes a powerful **daemon mode** (also called **watch mode**) that automatically monitors your source files for changes and triggers rebuilds whenever modifications are detected. This is particularly useful during active development when you want immediate feedback on your code changes.

## Features

- **Automatic File Monitoring**: Watches all C++ source files (`.cpp`, `.cc`, `.cxx`, `.c++`) and header files (`.h`, `.hpp`, `.hxx`, `.h++`)
- **Smart Watching**: Monitors source files, include directories, and platform-specific sources
- **Build Logging**: All builds are logged to a file with timestamps, build results, and error tracking
- **Configurable Polling**: Adjust the file check interval to suit your needs
- **Graceful Shutdown**: Press `Ctrl+C` to stop the daemon cleanly

## Usage

### Basic Usage

Start daemon mode with default settings (1 second polling interval):

```bash
bodge watch
```

Or use the `daemon` alias:

```bash
bodge daemon
```

### Custom Polling Interval

Set a custom polling interval (in milliseconds):

```bash
bodge watch --interval=2000    # Check every 2 seconds
bodge daemon --interval=500    # Check every 0.5 seconds
```

**Note**: Minimum polling interval is 100ms.

### Custom Log File

Specify a custom log file path:

```bash
bodge watch --log=my_build.log
```

### Combined Options

You can combine multiple options:

```bash
bodge daemon --interval=3000 --log=custom_log.log
```

## How It Works

1. **Initial Build**: When daemon mode starts, it performs an initial build of all configured targets
2. **File Monitoring**: The daemon then begins monitoring all source files and include directories
3. **Change Detection**: Every polling interval, the daemon checks if any files have been modified
4. **Automatic Rebuild**: When changes are detected, the daemon automatically triggers a rebuild
5. **Logging**: All build attempts, successes, and failures are logged with timestamps

## Build Logging

### Log File Location

By default, logs are written to `bodge_daemon.log` in the current directory.

### Log Format

The log file includes:
- Session start/end timestamps
- File changes that triggered builds
- Build start times
- Build results (success/failure)
- Build duration in milliseconds
- Build summaries (total builds, successes, failures)

### Example Log Output

```
[2025-10-20 14:32:15.123] ==========================================================
[2025-10-20 14:32:15.123] Bodge Daemon started
[2025-10-20 14:32:15.123] ==========================================================
[2025-10-20 14:32:15.124] [INFO] Daemon mode started
[2025-10-20 14:32:15.124] [INFO] Performing initial build
[2025-10-20 14:32:15.125] Build #1 started
[2025-10-20 14:32:18.456] Build #1 SUCCEEDED - Duration: 3331 ms
[2025-10-20 14:32:18.456] ----------------------------------------------------------
[2025-10-20 14:35:42.789] File changes detected:
[2025-10-20 14:35:42.789]   - src/main.cpp
[2025-10-20 14:35:42.890] Build #2 started
[2025-10-20 14:35:45.234] Build #2 SUCCEEDED - Duration: 2344 ms
[2025-10-20 14:35:45.234] ----------------------------------------------------------
```

## Watched Files

Daemon mode automatically watches:

- All source files defined in your targets
- Global include directories
- Target-specific include directories
- Platform-specific source files
- Legacy sources (if configured)

### File Types Monitored

- C++ source files: `.cpp`, `.cc`, `.cxx`, `.c++`
- C++ header files: `.h`, `.hpp`, `.hxx`, `.h++`

## Use Cases

### Active Development

Keep daemon mode running in a terminal while you edit code. Every time you save a file, your project automatically rebuilds:

```bash
bodge watch
```

### Continuous Integration Testing

Run daemon mode during development to catch build errors immediately:

```bash
bodge daemon --interval=1000 --log=ci_build.log
```

### Multiple Monitors

Run daemon mode on a secondary monitor to watch build status in real-time while coding on your primary monitor.

## Best Practices

1. **Set Appropriate Intervals**: 
   - For small projects: 500-1000ms works well
   - For large projects: 2000-5000ms to avoid constant rebuilding

2. **Monitor the Log File**: 
   - Check the log file to track build history and errors
   - Use it to identify patterns in build failures

3. **Graceful Shutdown**: 
   - Always stop daemon mode with `Ctrl+C` to ensure logs are properly closed
   - The daemon writes a summary when shutting down gracefully

4. **Exclude Build Artifacts**: 
   - Add `bodge_daemon.log` to your `.gitignore`
   - Don't commit daemon log files to version control

## Stopping Daemon Mode

Press `Ctrl+C` in the terminal where daemon mode is running. The daemon will:

1. Stop monitoring for file changes
2. Complete any in-progress operations
3. Write a session summary to the log
4. Exit gracefully

## Troubleshooting

### Daemon doesn't detect changes

- **Issue**: Changes to files aren't triggering rebuilds
- **Solution**: Check that your files are in directories being monitored (sources, includes). Verify file extensions are correct.

### Too many rebuilds

- **Issue**: Daemon triggers rebuilds too frequently
- **Solution**: Increase the polling interval with `--interval=<ms>`

### Build errors not logged

- **Issue**: Build errors don't appear in the log
- **Solution**: The daemon logs build results and timestamps. Check the console output for detailed compiler error messages.

### High CPU usage

- **Issue**: Daemon uses too much CPU
- **Solution**: Increase the polling interval to reduce file system checks

## Configuration

Daemon mode respects all settings in your `.bodge` configuration file:

- Target definitions
- Platform configurations
- Source file patterns
- Include directories
- Build sequences

All builds triggered by daemon mode use the same configuration as manual builds.

## Platform Support

Daemon mode works on:
- Windows (tested)
- Linux (tested)
- macOS (should work, uses standard C++17 filesystem APIs)

## Example Workflow

```bash
# Terminal 1: Start daemon mode
cd my_project
bodge watch --interval=1000

# Terminal 2: Edit your code
vim src/main.cpp
# Save changes...

# Terminal 1 automatically shows:
# [CHANGE DETECTED] 1 file(s) changed:
#   - src/main.cpp
# [INFO] Triggering rebuild...
# [SUCCESS] Rebuild completed successfully.
```

## See Also

- [Getting Started](getting-started.md)
- [Configuration Guide](configuration.md)
- [Examples](examples.md)

