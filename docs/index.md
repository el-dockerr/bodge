---
layout: default
title: Home
---

# Bodge - The Idiotic Build System

A minimalistic but powerful C++ build system that reads from a simple configuration file. It aims to be a replacement for CMAKE one day and make it more easy and reliable to build C++ and C projects.

> **bodge** [/bɒdʒ/ verb: make or repair (something) badly or clumsily.]

## 🚀 Quick Start

Getting started with Bodge is incredibly simple:

1. **Install Bodge** (download from [releases](https://github.com/el-dockerr/bodge/releases))
2. **Create a `.bodge` file** in your project root
3. **Run `bodge`** - that's it!

```bash
bodge              # Build all targets
bodge help         # Show help
bodge list         # List available targets and sequences
```

## ✨ Key Features

### 🎯 Multi-Target Support
- Build executables, shared libraries (DLLs/SOs), and static libraries
- Per-target configuration with global defaults
- Automatic file extension handling for different platforms

### 🔄 Build Sequences
- Chain multiple build and file operations
- Copy, remove, and create directory operations
- Perfect for deployment workflows

### 🛠️ Advanced Build System
- Cross-platform compatibility (Windows/Linux)
- Static linking support to eliminate DLL dependencies
- Comprehensive error handling and logging

### 📁 File Operations
- Built-in file and directory copying
- Safe file/directory removal
- Directory creation with parent path handling

## 📖 Documentation

- [Getting Started](getting-started.html) - Installation and first steps
- [Configuration Reference](configuration.html) - Complete `.bodge` file reference
- [Examples](examples.html) - Real-world usage examples
- [Contributing Guide](contributing.html) - How to contribute

## 🏗️ Building Bodge

### Using Bodge (preferred from now on)
```bash
bodge
```
Yes, it is that easy! Bodge can already be built by bodge itself.

### Using Make (Linux/macOS)
```bash
make all        # Build the project
make clean      # Clean build artifacts
make debug      # Build with debug symbols
```

### Using Windows (MinGW)
```bash
.\make.bat
```

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](contributing.html) for details.

## 📄 License

Bodge is licensed under the [GNU General Public License v3.0](https://github.com/el-dockerr/bodge/blob/main/LICENSE).

---

---

**Made with ❤️ by Swen "El Dockerr" Kalski**

[View on GitHub](https://github.com/el-dockerr/bodge) | [Download Latest Release](https://github.com/el-dockerr/bodge/releases)