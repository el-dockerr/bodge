#!/bin/bash
# Release script for Bodge
# Usage: ./scripts/create-release.sh <version>
# Example: ./scripts/create-release.sh v1.0.0

set -e

VERSION=$1

if [ -z "$VERSION" ]; then
    echo "Usage: $0 <version>"
    echo "Example: $0 v1.0.0"
    exit 1
fi

# Validate version format
if [[ ! $VERSION =~ ^v[0-9]+\.[0-9]+\.[0-9]+.*$ ]]; then
    echo "Error: Version must follow format v1.2.3 (with optional suffix like v1.2.3-beta)"
    exit 1
fi

echo "Creating release for version: $VERSION"

# Check if we're on main branch
CURRENT_BRANCH=$(git branch --show-current)
if [ "$CURRENT_BRANCH" != "main" ]; then
    echo "Warning: You're not on the main branch (currently on: $CURRENT_BRANCH)"
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Check for uncommitted changes
if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "Error: You have uncommitted changes. Please commit or stash them first."
    git status --short
    exit 1
fi

# Pull latest changes
echo "Pulling latest changes..."
git pull origin main

# Update version in source code (if you have version files)
# You can add version updates here if needed

# Build and test locally
echo "Building and testing locally..."
make clean
make all

# Test the build
echo "Testing the build..."
./bodge help
./bodge list

echo "Local build and test successful!"

# Create and push tag
echo "Creating and pushing tag: $VERSION"
git tag -a "$VERSION" -m "Release $VERSION"
git push origin "$VERSION"

echo "Release tag $VERSION has been created and pushed!"
echo "GitHub Actions will now build binaries for all platforms."
echo ""
echo "Next steps:"
echo "1. Go to https://github.com/el-dockerr/bodge/releases"
echo "2. Edit the release draft that was created"
echo "3. Add release notes describing changes"
echo "4. Publish the release"
echo ""
echo "Binaries will be automatically attached to the release!"