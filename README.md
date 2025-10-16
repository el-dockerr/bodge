# Bodge - The Idiotic Build System

A minimalistic but powerful C++ build system that reads from a simple configuration file.
It aims to be a replacement for CMAKE one day and make it more easy and reliable to build C++ and C projects.

bodge [/bɒdʒ/ verb: make or repair (something) badly or clumsily.]

[Find a full documentation here](https://el-dockerr.github.io/bodge)

This buildsystem is the draft to overcome typical issues of CMAKE. The vast of dependencies and the chaos it causes should be solved in a 
modern more package like approach. In Bodge you just point out the dependencies and what compiler to be used and the flags.
Everything straight forward without complex commands. Bodge also solves 3rd party dependencies with git support, where you can define sources, pull them 
and add them to the build chain. Other features will be added step by step.

The major goal is to have one little program what makes the build of C++ software easy like maven in Java or Cargo in Rust.


## Download

Pre-compiled binaries are available in [Releases](https://github.com/el-dockerr/bodge/releases) for Windows, Linux, and macOS.

## Building the Project

### Using bodge (preferred from know)
```bash
bodge
```
Yes it is that easy !!! Bodge can be already build by bodge

### Using Make
```bash
make all        # Build the project
make clean      # Clean build artifacts
make debug      # Build with debug symbols
make install    # Install to system (Unix/Linux)
```

### Using Windows (mingw)
```bash
./make.bat
```

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

### Manual Compilation
```bash
g++ -std=c++17 -Wall -Wextra -Isrc src/*.cpp -o bodge
```

## Key Features

### **Multi-Target Support**
- Build executables, shared libraries (DLLs/SOs), and static libraries
- Per-target configuration with global defaults
- Automatic file extension handling for different platforms

### **Build Sequences**
- Chain multiple build and file operations
- Copy, remove, and create directory operations
- Perfect for deployment workflows

### **Advanced Build System**
- Cross-platform compatibility (Windows/Linux)
- Static linking support to eliminate DLL dependencies
- Comprehensive error handling and logging

### **Automatic Source Collection**
- Use `src/**` to automatically collect all C++ files from directories
- Recursive pattern matching with `**` and single directory with `*`
- Automatic dependency analysis and build order optimization
- No need to manually list every source file

### **File Operations**
- Built-in file and directory copying
- Safe file/directory removal
- Directory creation with parent path handling

### **fetch sources from other repositories**
- No hassle about fetch and pull
- Dependencies can setup fluently

## Benefits of This Modular Design

1. **Single Responsibility Principle**: Each class has one clear purpose
2. **Testability**: Individual components can be unit tested in isolation
3. **Maintainability**: Changes to one component don't affect others
4. **Reusability**: Utility classes can be reused in other projects
5. **Readability**: Code is organized logically and easy to navigate
6. **Extensibility**: New features can be added without major refactoring
7. **Multi-Target Support**: Build multiple outputs from one configuration
8. **Workflow Automation**: Sequences enable complex build and deploy workflows

## Usage

### Basic Usage
Create a `.bodge` configuration file and run:

```bash
./bodge              # Build all targets
./bodge help         # Show help
./bodge list         # List available targets and sequences
```

### Advanced Usage with Targets and Sequences

```bash
./bodge build mylib           # Build specific target
./bodge sequence deploy       # Execute specific sequence
```

### Configuration Examples

#### Simple Configuration with Automatic Source Collection:
```
name: MyProject
compiler: g++
output_name: my_app
cxx_flags: -std=c++17, -Wall, -O2
sources: src/**
include_dirs: include
libraries: pthread, m
```

#### Legacy Configuration (Manual File Listing):
```
name: MyProject
compiler: g++
output_name: my_app
cxx_flags: -std=c++17, -Wall, -O2
sources: main.cpp, utils.cpp
include_dirs: include
libraries: pthread, m
```

#### Advanced Multi-Target Configuration:
```
name: Multi-Target Project
compiler: g++

# Global settings for all targets
global_cxx_flags: -std=c++17, -Wall, -static-libgcc, -static-libstdc++
global_include_dirs: include

# Main executable target (automatic source collection)
main.type: exe
main.output_name: my_app
main.sources: src/**
main.libraries: mylib

# Shared library target (automatic source collection)
mylib.type: shared
mylib.output_name: mylib
mylib.sources: src/lib/**
mylib.cxx_flags: -O2, -fPIC

# Static library target (automatic source collection)
tools.type: static
tools.output_name: tools
tools.sources: src/tools/**

# Build sequence
sequence.build_all: build:mylib build:tools build:main

# Deploy sequence with file operations
sequence.deploy: build:main mkdir:dist copy:my_app.exe->dist/my_app.exe copy:config->dist/config

# Clean sequence
sequence.clean: remove:*.exe remove:*.dll remove:dist
```

### Supported Target Types
- `exe` / `executable` - Executable programs (.exe on Windows)
- `shared` / `dll` / `so` - Shared libraries (.dll on Windows, .so on Linux)
- `static` / `lib` - Static libraries (.lib on Windows, .a on Linux)

### Supported Operations in Sequences
- `build:target_name` - Build a specific target
- `copy:source->destination` - Copy files or directories  
- `remove:path` - Remove files or directories
- `mkdir:path` - Create directories

## License

GNU General Public License v3.0