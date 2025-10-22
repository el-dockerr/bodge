---
layout: default
title: Automatic Source Collection
---

# Automatic Source Collection in Bodge

Bodge now supports automatic source file collection using wildcard patterns, making it easy to build C++ projects without manually listing every source file.

## Supported Patterns

### Recursive Collection (`**`)
Collects all C++ source files from the specified directory and all subdirectories:
```
sources: src/**
```

This will automatically find and include all `.cpp`, `.cxx`, `.cc`, and `.c++` files in:
- `src/`
- `src/subdir1/`
- `src/subdir2/`
- `src/subdir1/nested/`
- etc.

### Single Directory Collection (`*`)
Collects C++ source files from the specified directory only (no subdirectories):
```
sources: src/*
```

### Mixed Patterns
You can combine automatic collection with specific files:
```
sources: src/main.cpp, src/utils/**, src/core/*
```

## Dependency Analysis

When using automatic source collection, Bodge automatically:

1. **Analyzes header dependencies** - Scans `#include "..."` statements in source files
2. **Determines build order** - Uses topological sorting to ensure files are compiled in the correct dependency order
3. **Prevents circular dependencies** - Warns about circular dependencies and falls back to alphabetical order

## Examples

### Simple Project
```bodge
name: My Project
compiler: g++
sources: src/**
output_name: my_app
```

### Multi-Target Project
```bodge
# Main executable with all sources
main.type: exe
main.output_name: my_app
main.sources: src/**

# Shared library from specific subdirectory  
utils.type: shared
utils.output_name: utils
utils.sources: src/lib/**

# Static library with mixed sources
tools.type: static
tools.output_name: tools
tools.sources: src/tools/**, src/common/helper.cpp
```

## File Extensions Supported

**Source Files (automatically collected):**
- `.cpp` (C++ source)
- `.cxx` (C++ source)
- `.cc` (C source)
- `.c++` (C++ source)

**Header Files (for dependency analysis):**
- `.h` (C/C++ header)
- `.hpp` (C++ header)
- `.hxx` (C++ header)
- `.h++` (C++ header)

## Benefits

1. **No Manual File Listing** - Just specify `src/**` instead of listing every source file
2. **Automatic Updates** - New files are automatically included without updating the build configuration
3. **Correct Build Order** - Dependencies are analyzed automatically to ensure proper compilation order
4. **Cross-Platform** - Works on Windows, Linux, and macOS
5. **Backwards Compatible** - Still supports explicit file listing for precise control

## Migration from Manual Listing

**Before:**
```bodge
sources: src/main.cpp, src/app.cpp, src/utils/helper.cpp, src/utils/math.cpp, src/core/engine.cpp
```

**After:**
```bodge
sources: src/**
```

The new automatic collection will find all the same files plus any new ones you add, and compile them in the correct dependency order.