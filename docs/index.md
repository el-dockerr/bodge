---
layout: default
title: Home
---

# Bodge - The Idiotic Build System

A minimalistic but powerful C++ build system that reads from a simple configuration file. It aims to be a replacement for CMAKE one day and make it more easy and reliable to build C++ and C projects.

> **bodge** [/bɒdʒ/ verb: make or repair (something) badly or clumsily.]

## Quick Start

Getting started with Bodge is incredibly simple:

1. **Install Bodge** (download from [releases](https://github.com/el-dockerr/bodge/releases))
2. **Create a `.bodge` file** in your project root
3. **Run `bodge`** - that's it!

```bash
bodge              # Build all targets
bodge help         # Show help
bodge list         # List available targets and sequences
bodge version      # Version
```

## Key Features

<div class="feature-grid">
  <div class="feature-card">
    <h3>🔗 Repository Integration</h3>
    <p>Fetch sources from external repositories and build them directly. Keeps your dependencies always up to date.</p>
  </div>

  <div class="feature-card">
    <h3>🎯 Multi-Target Support</h3>
    <p>Build executables, shared libraries (DLLs/SOs), and static libraries with per-target configuration and automatic platform handling.</p>
  </div>

  <div class="feature-card">
    <h3>⚡ Build Sequences</h3>
    <p>Chain multiple build and file operations. Perfect for complex deployment workflows with copy, remove, and mkdir operations.</p>
  </div>

  <div class="feature-card">
    <h3>📂 Smart Source Collection</h3>
    <p>Use <code>src/**</code> to automatically collect all C++ files. Recursive pattern matching means no manual file listing.</p>
  </div>

  <div class="feature-card">
    <h3>🌐 Cross-Platform</h3>
    <p>Works seamlessly on Windows and Linux with platform-specific configurations and static linking support.</p>
  </div>

  <div class="feature-card">
    <h3>🛠️ File Operations</h3>
    <p>Built-in file and directory copying, safe removal, and directory creation with full parent path handling.</p>
  </div>
</div>

## Documentation

<div class="feature-grid">
  <div class="feature-card">
    <h3>🚀 Getting Started</h3>
    <p>Installation and first steps to get Bodge running in minutes.</p>
    <a href="getting-started" class="btn">Get Started →</a>
  </div>

  <div class="feature-card">
    <h3>⚙️ Configuration</h3>
    <p>Complete <code>.bodge</code> file reference with all available options.</p>
    <a href="configuration" class="btn">Learn More →</a>
  </div>

  <div class="feature-card">
    <h3>📁 Auto Source Collection</h3>
    <p>Use <code>src/**</code> patterns for automatic file discovery.</p>
    <a href="automatic-source-collection" class="btn">Explore →</a>
  </div>

  <div class="feature-card">
    <h3>🌍 Internationalization</h3>
    <p>Multi-language support for your build system.</p>
    <a href="internationalization" class="btn">Learn More →</a>
  </div>

  <div class="feature-card">
    <h3>👁️ Daemon Mode</h3>
    <p>Automatic rebuilds with intelligent file watching.</p>
    <a href="daemon-mode" class="btn">Explore →</a>
  </div>

  <div class="feature-card">
    <h3>📚 Examples</h3>
    <p>Real-world usage examples and best practices.</p>
    <a href="examples" class="btn">View Examples →</a>
  </div>

  <div class="feature-card">
    <h3>🤝 Contributing</h3>
    <p>Help improve Bodge and join our community.</p>
    <a href="contributing" class="btn">Contribute →</a>
  </div>

  <div class="feature-card">
    <h3>💻 Windows Setup</h3>
    <p>Configure Windows Terminal for the best experience.</p>
    <a href="windows-terminal-setup" class="btn">Setup Guide →</a>
  </div>
</div>

## Building Bodge

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

## Contributing

We welcome contributions! Please see our [Contributing Guide](contributing.html) for details.

## License

Bodge is licensed under the [Bodge License (BL-V1.0)](https://github.com/el-dockerr/bodge/blob/main/license.txt).
