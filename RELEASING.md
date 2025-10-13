# Release Process for Bodge

This document describes how to create releases with automatic binary generation for Bodge.

## Automated Binary Releases

When you create a release, GitHub Actions automatically builds binaries for:
- **Windows (x64)** - `bodge-windows-x64.zip`
- **Linux (x64)** - `bodge-linux-x64.tar.gz`  
- **macOS (x64)** - `bodge-macos-x64.tar.gz`

Each archive contains:
- The `bodge` executable (or `bodge.exe` on Windows)
- `README.md` with project information
- `example.bodge` configuration file
- `.bodge` file used to build Bodge itself

## Creating a Release

### Method 1: Using the Release Scripts (Recommended)

#### On Windows:
```cmd
scripts\create-release.bat v1.0.0
```

#### On Linux/macOS:
```bash
chmod +x scripts/create-release.sh
./scripts/create-release.sh v1.0.0
```

The script will:
1. ✅ Validate version format
2. ✅ Check for uncommitted changes
3. ✅ Pull latest changes from main
4. ✅ Build and test locally
5. ✅ Create and push the git tag
6. ✅ Trigger automated binary builds

### Method 2: Manual Process

1. **Prepare the release**
   ```bash
   # Ensure you're on main branch and up to date
   git checkout main
   git pull origin main
   
   # Build and test locally
   make clean && make all
   ./bodge help  # Test the build
   ```

2. **Create and push tag**
   ```bash
   git tag -a v1.0.0 -m "Release v1.0.0"
   git push origin v1.0.0
   ```

3. **Complete the release on GitHub**
   - Go to https://github.com/el-dockerr/bodge/releases
   - Edit the auto-created draft release
   - Add release notes
   - Publish the release

## Release Workflow Details

### What Happens Automatically

1. **Trigger**: When you push a tag starting with `v` or publish a GitHub release

2. **Build Matrix**: 
   - Ubuntu Latest (Linux x64)
   - Windows Latest (Windows x64) 
   - macOS Latest (macOS x64)

3. **Build Process**:
   - Install platform-specific dependencies
   - Build using `make` (Linux/macOS) or `make.bat` (Windows)
   - Test basic functionality (`bodge help`, `bodge list`)
   - Strip debug symbols (Linux/macOS) to reduce file size
   - Create archives with binary and documentation

4. **Upload Process**:
   - Upload binaries as GitHub release assets
   - Generate SHA256 checksums for verification
   - Attach checksums file to the release

### Archive Contents

Each platform archive contains:
```
bodge-{platform}-x64/
├── bodge(.exe)           # The main executable
├── README.md             # Project documentation
├── example.bodge         # Example configuration
└── .bodge               # Bodge's own build config
```

### Checksum Verification

A `checksums.txt` file is generated with SHA256 hashes:
```
a1b2c3d4... bodge-windows-x64.zip
e5f6g7h8... bodge-linux-x64.tar.gz
i9j0k1l2... bodge-macos-x64.tar.gz
```

Users can verify downloads:
```bash
# Linux/macOS
sha256sum -c checksums.txt

# Windows PowerShell
Get-FileHash bodge-windows-x64.zip -Algorithm SHA256
```

## Version Numbering

Follow semantic versioning (SemVer):
- **v1.0.0** - Major release with breaking changes
- **v1.1.0** - Minor release with new features
- **v1.0.1** - Patch release with bug fixes
- **v1.0.0-beta** - Pre-release versions

## Release Notes Template

```markdown
## What's New in v1.0.0

### 🚀 New Features
- Added multi-target support for libraries and executables
- Implemented build sequences for complex workflows
- Cross-platform file operations (copy, remove, mkdir)

### 🔧 Improvements  
- Better error messages and logging
- Improved Windows compatibility
- Static linking support to eliminate DLL dependencies

### 🐛 Bug Fixes
- Fixed configuration parsing edge cases
- Resolved path handling issues on Windows

### 📦 Downloads
The binaries are automatically attached to this release:
- Windows: `bodge-windows-x64.zip`
- Linux: `bodge-linux-x64.tar.gz` 
- macOS: `bodge-macos-x64.tar.gz`

Verify downloads using the `checksums.txt` file.
```

## Troubleshooting

### Build Failures
- Check the Actions tab for detailed error logs
- Ensure all platforms can build (test locally first)
- Verify dependencies are correctly installed in the workflow

### Missing Binaries
- Confirm the release workflow completed successfully
- Check that the tag follows the `v*` pattern
- Ensure the release was published (not just drafted)

### Checksum Mismatches
- Re-download the file (may be corrupted)
- Check if you downloaded the correct file
- Report issues if checksums consistently fail

---

For more information, see the [GitHub Actions workflows](../.github/workflows/) in this repository.