---
layout: default
title: Contributing - Bodge
---

# Contributing to Bodge

We welcome contributions from the community! This guide will help you get started.

## Ways to Contribute

- **Report bugs** - Found an issue? Let us know!
- **Suggest features** - Have an idea for improvement?
- **Submit code** - Fix bugs or implement new features
- **Improve documentation** - Help make our docs better
- **Write tests** - Help us improve code quality

## Getting Started

### 1. Fork and Clone
```bash
# Fork the repository on GitHub, then:
git clone https://github.com/YOUR_USERNAME/bodge.git
cd bodge
```

### 2. Build and Test
```bash
# Build Bodge
make all  # Linux/macOS
# or 
.\make.bat  # Windows

# Test the build
./bodge help
```

### 3. Make Your Changes
- Create a feature branch: `git checkout -b feature/my-feature`
- Make your changes
- Test your changes thoroughly
- Commit with clear messages

### 4. Submit a Pull Request
- Push to your fork: `git push origin feature/my-feature`
- Create a pull request on GitHub
- Describe your changes clearly

## Development Guidelines

### Code Style
- Follow existing code conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and small

### C++ Standards
- Use C++17 features appropriately
- Prefer standard library over third-party dependencies
- Handle errors gracefully
- Use RAII principles

### Testing
- Test your changes on multiple platforms if possible
- Create test cases for new features
- Ensure existing functionality isn't broken

## Architecture Overview

Understanding Bodge's architecture will help you contribute effectively:

```
src/
├── main.cpp           # Entry point and CLI handling
├── ProjectConfig.*    # Configuration data structures
├── ConfigParser.*     # .bodge file parsing
├── BuildSystem.*      # Build orchestration and execution
└── StringUtils.*      # Utility functions
```

### Key Classes

1. **ConfigParser** - Parses `.bodge` configuration files
2. **ProjectConfig** - Holds all project configuration data
3. **BuildSystem** - Orchestrates builds and file operations
4. **StringUtils** - String manipulation utilities

## Adding New Features

### New Configuration Options
1. Add the option to `ProjectConfig` struct
2. Update `ConfigParser` to handle the new option
3. Update `BuildSystem` to use the option
4. Add documentation and examples

### New Operations for Sequences
1. Add new `OperationType` enum value
2. Update `ConfigParser::process_sequence_config_line()`
3. Implement the operation in `BuildSystem::execute_operation()`
4. Add tests and documentation

### New Target Types
1. Add new `BuildType` enum value
2. Update `ConfigParser::parse_build_type()`
3. Update `BuildTarget::get_output_extension()`
4. Update `BuildSystem::generate_target_command()` for any special handling

## Submitting Issues

When reporting bugs, please include:

- **Bodge version** - Run `bodge help` to see version info
- **Operating system** - Windows, Linux, macOS, etc.
- **Compiler version** - `g++ --version` or `clang --version`
- **Sample .bodge file** - Minimal reproduction case
- **Expected vs actual behavior**
- **Error messages** - Full error output if applicable

### Example Issue Template
```
**Environment:**
- OS: Windows 11 / Ubuntu 22.04 / macOS 13
- Compiler: g++ 11.2.0
- Bodge version: 1.0.0

**Description:**
Brief description of the issue

**Steps to reproduce:**
1. Create .bodge file with: ...
2. Run: bodge build mylib
3. Observe error: ...

**Expected behavior:**
Should build successfully

**Actual behavior:**
Fails with error: ...

**Sample .bodge file:**
```
name: Test Project
mylib.type: shared
mylib.sources: test.cpp
```
```

## Feature Requests

For feature requests, please describe:

- **Use case** - What problem does this solve?
- **Proposed solution** - How should it work?
- **Examples** - Show example usage
- **Alternatives** - Other ways to achieve the same goal

## Documentation

Help us improve our documentation:

- Fix typos and grammar
- Add missing examples
- Clarify confusing sections
- Add tutorials for common use cases

Documentation files are in the `docs/` directory and use Markdown format.

## Code of Conduct

Please be respectful and constructive in all interactions. We want Bodge to have a welcoming community for everyone.

## Questions?

- **GitHub Issues** - For bugs and feature requests
- **GitHub Discussions** - For questions and general discussion
- **Email** - kalski.swen@gmail.com for private matters

Thank you for contributing to Bodge! 🚀