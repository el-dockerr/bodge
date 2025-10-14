@echo off
REM make.bat - Build script for Bodge Build System on Windows
REM Usage: make.bat [target]
REM Targets: all, clean, debug, help

set CXX=g++
set CXXFLAGS=-std=c++17 -Wall -Wextra -Wpedantic -O2 -static-libgcc -static-libstdc++
set TARGET=bodge.exe
set SRCDIR=src
set SOURCES=%SRCDIR%\core.cpp %SRCDIR%\main.cpp %SRCDIR%\ProjectConfig.cpp %SRCDIR%\StringUtils.cpp %SRCDIR%\ConfigParser.cpp %SRCDIR%\BuildSystem.cpp %SRCDIR%\git.cpp

REM Check if first argument is provided, default to "all"
if "%1"=="" (
    set ACTION=all
) else (
    set ACTION=%1
)

REM Execute the requested action
if /i "%ACTION%"=="all" goto build
if /i "%ACTION%"=="build" goto build
if /i "%ACTION%"=="clean" goto clean
if /i "%ACTION%"=="debug" goto debug
if /i "%ACTION%"=="help" goto help
if /i "%ACTION%"=="?" goto help

echo Unknown target: %ACTION%
echo Use 'make.bat help' to see available targets.
goto end

:build
echo Building %TARGET%...
%CXX% %CXXFLAGS% -I%SRCDIR% %SOURCES% -o %TARGET%
if %ERRORLEVEL% equ 0 (
    echo Build successful! Executable: %TARGET%
) else (
    echo Build failed with error code %ERRORLEVEL%
)
goto end

:clean
echo Cleaning build artifacts...
if exist "%TARGET%" (
    del "%TARGET%"
    echo Removed %TARGET%
) else (
    echo %TARGET% not found, nothing to clean.
)
if exist "*.o" (
    del "*.o"
    echo Removed object files.
)
if exist "%SRCDIR%\*.o" (
    del "%SRCDIR%\*.o"
    echo Removed source object files.
)
echo Clean complete.
goto end

:debug
echo Building %TARGET% with debug symbols...
%CXX% %CXXFLAGS% -g -DDEBUG -I%SRCDIR% %SOURCES% -o %TARGET%
if %ERRORLEVEL% equ 0 (
    echo Debug build successful! Executable: %TARGET%
) else (
    echo Debug build failed with error code %ERRORLEVEL%
)
goto end

:help
echo Available targets:
echo   all      - Build the project (default)
echo   build    - Same as 'all'
echo   clean    - Remove build artifacts
echo   debug    - Build with debug symbols and DEBUG macro
echo   help     - Show this help message
echo.
echo Usage: make.bat [target]
echo Example: make.bat all
echo Example: make.bat clean
goto end

:end