---
layout: default
title: Examples - Bodge
---

# Examples

This page shows real-world examples of using Bodge for different types of projects.

## Simple Executable

The most basic use case - building a single executable:

**.bodge:**
```
name: Simple Calculator
compiler: g++
output_name: calc
cxx_flags: -std=c++17, -Wall, -O2
sources: src/main.cpp, src/calculator.cpp
include_dirs: include
```

**Usage:**
```bash
bodge           # Builds calc.exe (Windows) or calc (Linux)
```

## Multi-Target Library Project

A project that builds both a library and an executable that uses it:

**.bodge:**
```
name: Math Library Project
compiler: g++

# Global settings
global_cxx_flags: -std=c++17, -Wall
global_include_dirs: include

# Math library (shared)
mathlib.type: shared
mathlib.output_name: mathlib
mathlib.sources: src/lib/vector.cpp, src/lib/matrix.cpp, src/lib/algorithms.cpp
mathlib.cxx_flags: -O3, -fPIC

# Test executable
test.type: exe
test.output_name: math_test
test.sources: src/test/main.cpp, src/test/test_vector.cpp
test.libraries: mathlib

# Benchmark executable  
bench.type: exe
bench.output_name: math_bench
bench.sources: src/bench/benchmark.cpp
bench.libraries: mathlib
bench.cxx_flags: -O3, -DNDEBUG
```

**Usage:**
```bash
bodge                    # Build all targets
bodge build mathlib      # Build only the library
bodge build test         # Build only the test executable
```

## Game Development Project

A more complex project with multiple libraries and deployment:

**.bodge:**
```
name: Space Shooter Game
compiler: g++

# Global settings for the entire project
global_cxx_flags: -std=c++17, -Wall, -Wextra
global_include_dirs: include, external/SDL2/include, external/OpenGL/include
global_library_dirs: external/SDL2/lib, external/OpenGL/lib

# Game engine library
engine.type: shared
engine.output_name: game_engine
engine.sources: src/engine/renderer.cpp, src/engine/audio.cpp, src/engine/physics.cpp
engine.cxx_flags: -O2, -fPIC
engine.libraries: SDL2, SDL2_mixer, GL

# Game logic library
game.type: static
game.output_name: game_logic
game.sources: src/game/player.cpp, src/game/enemies.cpp, src/game/weapons.cpp
game.libraries: engine

# Main executable
spaceshooter.type: exe
spaceshooter.output_name: spaceshooter
spaceshooter.sources: src/main.cpp, src/menu.cpp
spaceshooter.libraries: game_logic, engine
spaceshooter.cxx_flags: -O2

# Level editor tool
editor.type: exe
editor.output_name: level_editor
editor.sources: src/tools/editor.cpp, src/tools/ui.cpp
editor.libraries: engine

# Build sequences
sequence.game_only: build:engine build:game build:spaceshooter
sequence.tools: build:engine build:editor
sequence.all: build:engine build:game build:spaceshooter build:editor

# Deployment sequence
sequence.deploy: build:spaceshooter mkdir:release copy:spaceshooter.exe->release/ copy:game_engine.dll->release/ copy:assets->release/assets copy:config.ini->release/

# Development sequence
sequence.dev: build:spaceshooter copy:spaceshooter.exe->debug/ copy:game_engine.dll->debug/

# Clean everything
sequence.clean: remove:*.exe remove:*.dll remove:*.lib remove:release remove:debug
```

**Usage:**
```bash
bodge sequence game_only     # Build just the game
bodge sequence deploy        # Build and create release package
bodge sequence dev           # Build and copy to debug folder
bodge sequence clean         # Clean all build artifacts
```

## Cross-Platform Project

A project configured to work on both Windows and Linux:

**.bodge:**
```
name: Cross-Platform Utility
compiler: g++

# Cross-platform flags
global_cxx_flags: -std=c++17, -Wall, -static-libgcc, -static-libstdc++
global_include_dirs: include

# Core library (works on all platforms)
core.type: static
core.output_name: core
core.sources: src/core/file_utils.cpp, src/core/string_utils.cpp, src/core/config.cpp

# Platform-specific network library
network.type: shared
network.output_name: network
network.sources: src/network/common.cpp, src/network/socket.cpp
network.include_dirs: src/network
network.cxx_flags: -O2, -fPIC

# Main application
app.type: exe
app.output_name: myapp
app.sources: src/main.cpp, src/app.cpp
app.libraries: core, network

# Sequences
sequence.build_static: build:core build:app
sequence.build_all: build:core build:network build:app
sequence.package_windows: build:app copy:myapp.exe->dist/windows/ copy:network.dll->dist/windows/
sequence.package_linux: build:app copy:myapp->dist/linux/ copy:libnetwork.so->dist/linux/
```

## Plugin System

A project that builds a main application and multiple plugins:

**.bodge:**
```
name: Plugin System Demo
compiler: g++

global_cxx_flags: -std=c++17, -Wall, -fPIC
global_include_dirs: include, plugin_api

# Plugin API (shared library)
plugin_api.type: shared
plugin_api.output_name: plugin_api
plugin_api.sources: src/api/plugin_interface.cpp, src/api/plugin_manager.cpp
plugin_api.cxx_flags: -O2

# Main application
app.type: exe
app.output_name: demo_app
app.sources: src/main.cpp, src/application.cpp
app.libraries: plugin_api, dl

# Audio plugin
audio_plugin.type: shared
audio_plugin.output_name: audio_plugin
audio_plugin.sources: src/plugins/audio/audio_plugin.cpp
audio_plugin.libraries: plugin_api

# Graphics plugin
gfx_plugin.type: shared
gfx_plugin.output_name: graphics_plugin
gfx_plugin.sources: src/plugins/graphics/gfx_plugin.cpp
gfx_plugin.libraries: plugin_api, GL

# Network plugin
net_plugin.type: shared
net_plugin.output_name: network_plugin
net_plugin.sources: src/plugins/network/net_plugin.cpp
net_plugin.libraries: plugin_api

# Sequences
sequence.api: build:plugin_api
sequence.app: build:plugin_api build:app
sequence.plugins: build:plugin_api build:audio_plugin build:gfx_plugin build:net_plugin
sequence.all: build:plugin_api build:app build:audio_plugin build:gfx_plugin build:net_plugin
sequence.install: sequence:all mkdir:plugins copy:demo_app.exe->. copy:plugin_api.dll->. copy:audio_plugin.dll->plugins/ copy:graphics_plugin.dll->plugins/ copy:network_plugin.dll->plugins/
```

**Usage:**
```bash
bodge sequence app           # Build just the main app
bodge sequence plugins       # Build just the plugins  
bodge sequence install       # Build everything and set up directory structure
```

These examples demonstrate the flexibility and power of Bodge for managing complex C++ projects with multiple targets and deployment workflows.