cmake_minimum_required(VERSION 3.10)
project(bodge VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(bodge
    src/main.cpp
    src/ProjectConfig.cpp
    src/StringUtils.cpp
    src/ConfigParser.cpp
    src/BuildSystem.cpp
)

# Include directories
target_include_directories(bodge PRIVATE src)

# Set compiler flags
target_compile_options(bodge PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)