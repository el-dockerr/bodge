---
layout: default
title: Getting Started
---

# Getting Started with Bodge

## Installation

### Download Pre-built Binaries
1. Go to the [Releases page](https://github.com/el-dockerr/bodge/releases)
2. Download the appropriate binary for your platform
3. Add it to your system PATH

### Build from Source

#### Prerequisites
- GCC or Clang compiler with C++17 support
- Git (for cloning the repository)

#### Quick Build
```bash
git clone https://github.com/el-dockerr/bodge.git
cd bodge
make all  # Linux/macOS
# or
.\make.bat  # Windows
```

## Your First Project

Let's create a simple "Hello World" project:

### 1. Create Project Structure
```
my-project/
├── .bodge
└── src/
    └── main.cpp
```

### 2. Create main.cpp
```cpp
#include <iostream>

int main() {
    std::cout << "Hello, Bodge!" << std::endl;
    return 0;
}
```

### 3. Create .bodge Configuration
```
name: Hello World
compiler: g++
output_name: hello
cxx_flags: -std=c++17, -Wall, -O2
sources: src/main.cpp
```

### 4. Build and Run
```bash
cd my-project
bodge              # Build the project
./hello            # Run the executable (Linux/macOS)
# or
hello.exe          # Run the executable (Windows)
```

## Next Steps

<div class="feature-grid">
  <div class="feature-card">
    <h3>⚙️ Configuration Reference</h3>
    <p>Learn about all configuration options</p>
    <a href="configuration" class="btn">Read More →</a>
  </div>

  <div class="feature-card">
    <h3>📚 Examples</h3>
    <p>See more complex project examples</p>
    <a href="examples" class="btn">View Examples →</a>
  </div>

  <div class="feature-card">
    <h3>🤝 Contributing Guide</h3>
    <p>Help improve Bodge</p>
    <a href="contributing" class="btn">Contribute →</a>
  </div>
</div>

## Common Issues

### Compiler Not Found
If you get "compiler not found" errors:

1. **Install a C++ compiler:**
   - **Linux:** `sudo apt-get install build-essential` (Ubuntu/Debian)
   - **macOS:** Install Xcode Command Line Tools
   - **Windows:** Install MinGW-w64 or Visual Studio

2. **Specify compiler in .bodge:**
   ```
   compiler: /path/to/g++
   ```

### Missing Dependencies
For projects using external libraries, make sure to:
- Add include directories: `include_dirs: /path/to/headers`
- Add library directories: `library_dirs: /path/to/libs`
- Link libraries: `libraries: pthread, m`