---
layout: default
title: Configuration Reference
---

# Configuration Reference

This page documents all available options for `.bodge` configuration files.

## Global Settings

### Project Information
- `name`: Project name (string)
- `compiler`: C++ compiler to use (default: `g++`)

### Global Compilation Settings
Applied to all targets unless overridden:

- `global_cxx_flags`: Compiler flags for all targets
- `global_include_dirs`: Include directories for all targets  
- `global_library_dirs`: Library search directories for all targets
- `global_libraries`: Libraries to link for all targets

### Platform Configuration
- `platforms`: Default target platforms to build for (comma-separated)
  - If not specified, builds for current platform only
  - Example: `windows_x64, linux_x64, windows_x86`

### GIT Sub Resources

- `dependencies_url` : Source repositories to fetch (comma-separated)
- `dependencies_path` : Pathname where to clone the repositories (comma-separated)
- `run_bodge_after_clone` : when the root contains a .bodge file, shall it build as well (`true`, `false` - default: `false`) 

## Target Configuration

Targets are defined using the syntax: `targetname.property: value`

### Target Properties

#### Required
- `targetname.sources`: Source files to compile (comma-separated)

#### Optional
- `targetname.type`: Target type (`exe`, `shared`, `static`)
- `targetname.output_name`: Output filename (without extension)
- `targetname.cxx_flags`: Additional compiler flags
- `targetname.include_dirs`: Additional include directories
- `targetname.library_dirs`: Additional library directories
- `targetname.libraries`: Additional libraries to link
- `targetname.platforms`: Specific platforms this target should build for (comma-separated)

### Target Types

| Type | Description | Windows Extension | Linux Extension |
|------|-------------|------------------|-----------------|
| `exe` / `executable` | Executable program | `.exe` | (none) |
| `shared` / `dll` / `so` | Shared library | `.dll` | `.so` |
| `static` / `lib` | Static library | `.lib` | `.a` |

## Platform-Specific Configuration

Bodge supports architecture-aware building with platform-specific configurations.

### Platform Syntax

Platform configurations use the `@` symbol:
- `@platform.property: value` - Global platform-specific configuration
- `target@platform.property: value` - Target-specific platform configuration

### Supported Platforms

| OS | Architecture | Platform String |
|---|---|---|
| Windows | 32-bit | `windows_x86` |
| Windows | 64-bit | `windows_x64` |  
| Linux | 32-bit | `linux_x86` |
| Linux | 64-bit | `linux_x64` |
| Unix | 32-bit | `unix_x86` |
| Unix | 64-bit | `unix_x64` |
| Apple/macOS | 32-bit | `apple_x86` |
| Apple/macOS | 64-bit | `apple_x64` |
| Any OS | ARM 32-bit | `*_arm` |
| Any OS | ARM 64-bit | `*_arm64` |

You can also use partial matches:
- `@windows` - Matches all Windows platforms
- `@x64` - Matches all 64-bit platforms
- `@linux_x64` - Matches specifically Linux 64-bit

### Platform-Specific Properties

All standard target properties can be made platform-specific:
- `cxx_flags` - Platform-specific compiler flags
- `sources` - Platform-specific source files
- `include_dirs` - Platform-specific include directories
- `library_dirs` - Platform-specific library directories
- `libraries` - Platform-specific libraries
- `output_suffix` - Suffix to add to output name (e.g., `_x64`, `_win32`)

### Platform Examples

```
# Global platform-specific flags
@windows.cxx_flags: -static-libgcc, -static-libstdc++
@linux.cxx_flags: -pthread
@x86.cxx_flags: -m32
@x64.cxx_flags: -m64

# Target with platform-specific sources
mylib.sources: src/common.cpp
mylib@windows.sources: src/windows_impl.cpp
mylib@linux.sources: src/linux_impl.cpp

# Platform-specific libraries
app.libraries: common
app@windows.libraries: kernel32, user32
app@linux.libraries: dl, rt

# Platform-specific output names
app@windows_x64.output_suffix: _x64
app@windows_x86.output_suffix: _x86
```

To run the build only for a single platform or architecture, you can do it by adding the `--platform` or `--arch`.
Example:
```bash
bodge --platform=windows_x64   #to run only windows build
bodge --arch=x86 #to run a 32bit specific build.

```

## Sequences

Sequences define workflows with multiple operations:

```
sequence.name: operation1 operation2 operation3
```

### Available Operations

#### Build Operations
- `build:target_name` - Build specified target

#### File Operations
- `copy:source->destination` - Copy file or directory
- `remove:path` - Remove file or directory
- `mkdir:path` - Create directory

## Legacy Configuration

For backward compatibility, these legacy options are supported:

- `output_name`: Output filename (converted to default target)
- `cxx_flags`: Compiler flags (converted to default target)
- `sources`: Source files (converted to default target)
- `include_dirs`: Include directories (converted to default target)
- `library_dirs`: Library directories (converted to default target)
- `libraries`: Libraries to link (converted to default target)

## Complete Example

```
# Project information
name: Advanced C++ Project
compiler: g++

# Global settings
global_cxx_flags: -std=c++17, -Wall, -Wextra
global_include_dirs: include, external/headers

# Main executable
app.type: exe
app.output_name: my_application
app.sources: src/main.cpp, src/app.cpp, src/gui.cpp
app.libraries: mycore, myutils
app.cxx_flags: -O2

# Core library (shared)
mycore.type: shared
mycore.output_name: mycore
mycore.sources: src/core/engine.cpp, src/core/system.cpp
mycore.cxx_flags: -O3, -fPIC
mycore.libraries: pthread

# Utilities library (static)
myutils.type: static
myutils.output_name: myutils
myutils.sources: src/utils/helper.cpp, src/utils/math.cpp

# Build sequences
sequence.all: build:myutils build:mycore build:app
sequence.libs_only: build:myutils build:mycore
sequence.deploy: build:app mkdir:dist copy:my_application->dist/ copy:libmycore.so->dist/
sequence.clean: remove:*.exe remove:*.dll remove:*.so remove:*.lib remove:dist
```

## Comments and Formatting

- Lines starting with `#` are comments
- Empty lines are ignored
- Whitespace around colons and commas is automatically trimmed
- Values can contain spaces (useful for file paths with spaces)

## Variable Expansion

Currently, Bodge does not support variable expansion, but you can use absolute or relative paths:

```
# Relative paths (recommended)
mylib.sources: src/lib.cpp, src/helper.cpp
mylib.include_dirs: include, external/boost

# Absolute paths (if needed)
mylib.include_dirs: /usr/local/include, /opt/custom/include
```