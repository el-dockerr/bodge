---
layout: default
title: Configuration Reference
---

[← Getting Started](getting-started) | [Home](index) | [Examples →](examples)

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

### Target Types

| Type | Description | Windows Extension | Linux Extension |
|------|-------------|------------------|-----------------|
| `exe` / `executable` | Executable program | `.exe` | (none) |
| `shared` / `dll` / `so` | Shared library | `.dll` | `.so` |
| `static` / `lib` | Static library | `.lib` | `.a` |

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

---
[← Getting Started](getting-started) | [Home](index) | [Examples →](examples)