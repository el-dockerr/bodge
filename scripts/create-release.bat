@echo off
REM Release script for Bodge on Windows
REM Usage: scripts\create-release.bat <version>
REM Example: scripts\create-release.bat v1.0.0

setlocal enabledelayedexpansion

set VERSION=%1

if "%VERSION%"=="" (
    echo Usage: %0 ^<version^>
    echo Example: %0 v1.0.0
    exit /b 1
)

echo Creating release for version: %VERSION%

REM Check for uncommitted changes
git diff --quiet
if errorlevel 1 (
    echo Error: You have uncommitted changes. Please commit or stash them first.
    git status --short
    exit /b 1
)

git diff --cached --quiet
if errorlevel 1 (
    echo Error: You have staged changes. Please commit or stash them first.
    git status --short
    exit /b 1
)

REM Pull latest changes
echo Pulling latest changes...
git pull origin main

REM Build and test locally
echo Building and testing locally...
make.bat clean
make.bat all

REM Test the build
echo Testing the build...
bodge.exe help
bodge.exe list

echo Local build and test successful!

REM Create and push tag
echo Creating and pushing tag: %VERSION%
git tag -a "%VERSION%" -m "Release %VERSION%"
git push origin "%VERSION%"

echo Release tag %VERSION% has been created and pushed!
echo GitHub Actions will now build binaries for all platforms.
echo.
echo Next steps:
echo 1. Go to https://github.com/el-dockerr/bodge/releases
echo 2. Edit the release draft that was created
echo 3. Add release notes describing changes
echo 4. Publish the release
echo.
echo Binaries will be automatically attached to the release!