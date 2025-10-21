@echo off
REM Script to help fix Unicode display issues in Windows console
REM This script sets up your console for proper UTF-8 display

echo ========================================
echo Bodge Unicode Console Fix
echo ========================================
echo.

REM Set UTF-8 code page
echo [1/3] Setting console to UTF-8 (code page 65001)...
chcp 65001 >nul
if %ERRORLEVEL% equ 0 (
    echo     ✓ UTF-8 enabled successfully
) else (
    echo     X Failed to set UTF-8
)
echo.

REM Test Unicode characters
echo [2/3] Testing Unicode character display:
echo     Box drawing: ╔═══╗
echo                  ║ X ║
echo                  ╚═══╝
echo     Symbols: ✓ ✗ ℹ ⚠
echo     Emojis: 🔨 📦 ⚙️ 🎯
echo.

REM Show current settings
echo [3/3] Current console settings:
echo     Code Page: %CODEPAGE%
chcp
echo.

echo ========================================
echo Setup Complete!
echo ========================================
echo.
echo If you see the characters correctly above,
echo your console is ready to use Bodge!
echo.
echo To make this permanent:
echo   - For PowerShell: Add to your $PROFILE
echo     [Console]::OutputEncoding = [System.Text.Encoding]::UTF8
echo.
echo   - For CMD: Create a shortcut with:
echo     "chcp 65001 && cmd.exe"
echo.
echo Press any key to test Bodge...
pause >nul

REM Try to run bodge version if it exists
if exist "%~dp0..\bodge.exe" (
    echo.
    echo Running: bodge version
    echo ========================================
    "%~dp0..\bodge.exe" version
) else (
    echo.
    echo Note: bodge.exe not found in parent directory
    echo Please build Bodge first with: make.bat
)

echo.
echo Press any key to exit...
pause >nul

