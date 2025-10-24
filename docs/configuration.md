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

### Source File Support

Bodge supports both **C** and **C++** source files:
- `.c` files - C source files
- `.cpp`, `.cxx`, `.cc`, `.c++` files - C++ source files
- Use wildcards to include multiple file types: `src/**.[c,cpp]`

**Wildcard Patterns:**
- `**` - Recursive (all subdirectories)
- `*` - Single level (current directory only)
- `[c,cpp]` - Multiple extensions

**Examples:**
```
# C++ only (recursive)
myapp.sources: src/**.cpp

# C only (recursive)
myapp.sources: src/**.c

# Both C and C++ files (recursive)
myapp.sources: src/**.[c,cpp]

# Explicit list
myapp.sources: src/main.cpp, src/helper.c, src/utils.cpp

# Multiple folders with wildcards
myapp.sources: src/**, lib/**, external/helper.cpp

# Mix of recursive and non-recursive
myapp.sources: src/**, include/*, vendor/lib.c

# Different patterns for different folders
myapp.sources: cpp_code/**.cpp, c_code/**.c, common/**.[c,cpp]
```

**How Multiple Folders Work:**
When you specify multiple patterns separated by commas, Bodge will:
1. Expand each pattern independently
2. Collect all matching files
3. Remove duplicates automatically
4. Analyze dependencies to determine optimal build order

### Target Properties

#### Required
- `targetname.sources`: Source files to compile (comma-separated)
  - Supports `.c` and `.cpp` files
  - Can use wildcards: `**` for recursive, `*` for single level, `[c,cpp]` for extensions

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

# Main executable with multiple source folders
app.type: exe
app.output_name: my_application
app.sources: src/app/**, src/gui/**, src/main.cpp
app.libraries: mycore, myutils
app.cxx_flags: -O2

# Core library (shared) - using wildcard
mycore.type: shared
mycore.output_name: mycore
mycore.sources: src/core/**, platform/common/**.[c,cpp]
mycore.cxx_flags: -O3, -fPIC
mycore.libraries: pthread

# Platform-specific sources
mycore@windows.sources: platform/windows/**.cpp
mycore@linux.sources: platform/linux/**.cpp

# Utilities library (static) - mixed C and C++
myutils.type: static
myutils.output_name: myutils
myutils.sources: src/utils/**.cpp, src/legacy/**.c

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

## Configuration Validation and Troubleshooting

Bodge performs comprehensive validation of your `.bodge` file and provides detailed error messages when configuration is incomplete or invalid.

### Required Fields

#### Legacy Mode (Single Target)
At minimum, you need:
```
compiler: g++
output_name: myapp
sources: src/**.[c,cpp]   # Supports both .c and .cpp files
```

#### Multi-Target Mode
At minimum, you need:
```
compiler: g++

# Each target needs at least:
myapp.sources: src/**.[c,cpp]   # Supports both .c and .cpp files
myapp.output_name: myapp  # Optional, defaults to target name
```

### Common Validation Errors

#### Error: Missing compiler field
```
[FATAL] Configuration file '.bodge' is incomplete or invalid.

The following issues were found:
  - Missing 'compiler' field (e.g., compiler: g++)
```

**Solution**: Add a compiler specification to your `.bodge` file:
```
compiler: g++
```

#### Error: Missing sources for target
```
The following issues were found:
  - No valid build targets found. Each target needs:
    * Target 'myapp': missing 'sources' (e.g., myapp.sources: src/**.[c,cpp])
```

**Solution**: Add source files to your target:
```
myapp.sources: src/main.cpp, src/app.cpp
# or use wildcards for C++ files
myapp.sources: src/**.cpp
# or for both C and C++ files
myapp.sources: src/**.[c,cpp]
# or for C files only
myapp.sources: src/**.c
```

#### Error: Multiple missing fields
```
The following issues were found:
  - Missing 'compiler' field (e.g., compiler: g++)
  - Missing 'output_name' field (e.g., output_name: myapp)
  - Missing 'sources' field (e.g., sources: src/**.[c,cpp])
```

**Solution**: Add all required fields:
```
name: MyProject
compiler: g++
output_name: myapp
sources: src/**.[c,cpp]   # Include both C and C++ files
```

### Minimal Valid Configuration Examples

#### Minimal Legacy Configuration
```
compiler: g++
output_name: myapp
sources: src/main.cpp
```

#### Minimal Legacy Configuration with C Files
```
compiler: gcc
output_name: myapp
sources: src/main.c, src/utils.c
```

#### Minimal Configuration with Mixed C/C++ Sources
```
compiler: g++
output_name: myapp
sources: src/**.[c,cpp]   # Automatically includes both .c and .cpp files
```

#### Minimal Multi-Target Configuration
```
compiler: g++

myapp.sources: src/main.cpp
```

#### Multi-Target with C and C++ Targets
```
compiler: g++

# C library
mylib.sources: src/lib.c, src/helper.c
mylib.type: static
mylib.output_name: mylib

# C++ application
myapp.sources: src/main.cpp, src/app.cpp
myapp.libraries: mylib
```

#### Minimal Configuration with Platform Support
```
compiler: g++
output_name: myapp
sources: src/main.cpp
platforms: windows_x64, linux_x64

@windows_x64.cxx_flags: -static-libgcc, -static-libstdc++
```

### Validation Tips

1. **Start Simple**: Begin with a minimal configuration and add features incrementally
2. **Check Syntax**: Ensure each line follows the `key: value` format
3. **Watch Indentation**: While Bodge ignores leading/trailing whitespace, consistent formatting helps readability
4. **Use Comments**: Document your configuration with `#` comments
5. **Test Incrementally**: After adding new targets or settings, run `bodge list` to verify configuration