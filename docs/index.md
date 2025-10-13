---
layout: default
title: Bodge - The Idiotic Build System
---

# Bodge - The Idiotic Build System

<p class="lead">A minimalistic but powerful C++ build system that reads from a simple configuration file. It aims to be a replacement for CMAKE one day and make it more easy and reliable to build C++ and C projects.</p>

<div class="alert alert-info">
<strong>bodge</strong> [/bɒdʒ/ verb: make or repair (something) badly or clumsily.]
</div>

## Quick Start

Getting started with Bodge is incredibly simple:

1. **Install Bodge** (download from [releases](https://github.com/el-dockerr/bodge/releases))
2. **Create a `.bodge` file** in your project root
3. **Run `bodge`** - that's it!

```bash
bodge              # Build all targets
bodge help         # Show help
bodge list         # List available targets and sequences
```

## Key Features

<div class="row">
<div class="col-md-6">
<h3>Multi-Target Support</h3>
<ul>
<li>Build executables, shared libraries (DLLs/SOs), and static libraries</li>
<li>Per-target configuration with global defaults</li>
<li>Automatic file extension handling for different platforms</li>
</ul>
</div>

<div class="col-md-6">
<h3>Build Sequences</h3>
<ul>
<li>Chain multiple build and file operations</li>
<li>Copy, remove, and create directory operations</li>
<li>Perfect for deployment workflows</li>
</ul>
</div>
</div>

<div class="row">
<div class="col-md-6">
<h3>Advanced Build System</h3>
<ul>
<li>Cross-platform compatibility (Windows/Linux)</li>
<li>Static linking support to eliminate DLL dependencies</li>
<li>Comprehensive error handling and logging</li>
</ul>
</div>

<div class="col-md-6">
<h3>File Operations</h3>
<ul>
<li>Built-in file and directory copying</li>
<li>Safe file/directory removal</li>
<li>Directory creation with parent path handling</li>
</ul>
</div>
</div>

## 📖 Documentation

- [Getting Started](getting-started.html) - Installation and first steps
- [Configuration Reference](configuration.html) - Complete `.bodge` file reference
- [Examples](examples.html) - Real-world usage examples
- [API Reference](api.html) - Developer documentation

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

<div class="text-center">
<p><strong>Made with ❤️ by the Swen "El Dockerr" Kalski</strong></p>
<p>
<a href="https://github.com/el-dockerr/bodge" class="btn btn-primary">View on GitHub</a>
<a href="https://github.com/el-dockerr/bodge/releases" class="btn btn-success">Download</a>
</p>
</div>