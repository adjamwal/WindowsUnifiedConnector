# Local Build Guide: Windows Unified Connector

This guide explains how to set up and build Windows Unified Connector on your local Windows machine.

---

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Build Options](#build-options)
3. [Option A: Traditional Cisco Build](#option-a-traditional-cisco-build-requires-vpn)
4. [Option B: Conan-Based Build (POC)](#option-b-conan-based-build-poc)
5. [Building the Project](#building-the-project)
6. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required Software

#### 1. Visual Studio 2022
- **Edition**: Professional or Enterprise
- **Version**: 17.x
- **Toolset**: v143 (14.34.31933)

**Installation Steps**:
1. Download from: https://visualstudio.microsoft.com/downloads/
2. During installation, select:
   - ✅ **Desktop development with C++**
   - ✅ **Windows 10/11 SDK (10.0.22621.0)**
   - ✅ **C++ CMake tools** (optional but helpful)
   - ✅ **Test Adapter for Google Test**

**Verify Installation**:
```powershell
# Check Visual Studio version
"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" /?

# Check compiler version
"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.34.31933\bin\Hostx64\x64\cl.exe"
```

#### 2. Windows SDK
- **Version**: 10.0.22621.0 (comes with Visual Studio)
- **Location**: `C:\Program Files (x86)\Windows Kits\10`

**Verify**:
```powershell
dir "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0"
```

#### 3. Git
- Download from: https://git-scm.com/download/win
- Configure for line endings:
```bash
git config --global core.autocrlf true
```

#### 4. NuGet (for package restore)
```powershell
# Download NuGet
Invoke-WebRequest -Uri "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" -OutFile "$env:USERPROFILE\nuget.exe"

# Add to PATH
$env:PATH += ";$env:USERPROFILE"
```

#### 5. WiX Toolset (Optional - for installer builds)
- **Version**: 3.11+
- **Download**: https://wixtoolset.org/releases/
- **Install**: Run installer and Visual Studio extension

---

## Build Options

You have **two build options** depending on your environment:

| Option | Use Case | Requirements |
|--------|----------|--------------|
| **A. Traditional Cisco Build** | Production builds, full features | VPN to Cisco network, AmpRepositorySync access |
| **B. Conan-Based Build (POC)** | Public builds, no VPN needed | Conan 2.x, Internet access |

---

## Option A: Traditional Cisco Build (Requires VPN)

### Overview
This is the **original build method** using Cisco's internal dependency management (AmpRepositorySync).

### Prerequisites
- ✅ Connected to Cisco VPN (CSE-VPN)
- ✅ Access to https://nexus.engine.sourcefire.com
- ✅ AmpRepositorySync.exe available

### Step 1: Clone Repository
```bash
cd C:\Workspace
git clone https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector.git
cd WindowsUnifiedConnector
```

### Step 2: Initialize Submodules
```bash
git submodule update --init --recursive
```

### Step 3: Set Up Directory Structure
The project expects this structure:
```
C:\Workspace\
├── WindowsUnifiedConnector\     # This repo
└── common\                       # Dependencies (created by AmpRepositorySync)
    └── common-windows-build\
        ├── Release\
        │   └── x64\
        │       ├── include\      # Headers
        │       └── lib\          # Libraries
        └── Tools\
            └── signing\
```

### Step 4: Run AmpRepositorySync
```powershell
# Navigate to repo
cd C:\Workspace\WindowsUnifiedConnector

# Run AmpRepositorySync (from Jenkins or local copy)
# This reads build.spec and fetches all dependencies
AmpRepositorySync.exe checkout build.spec
```

**What this does**:
- Reads `build.spec` file
- Downloads ~20 prebuilt libraries from Cisco Nexus
- Places them in `C:\Workspace\common\common-windows-build\`

### Step 5: Update UCID
```powershell
# UCID is downloaded separately via PowerShell script
cd UCID
.\UpdateUCID.ps1
```

**Note**: Requires CSE-VPN connection to Nexus.

### Step 6: Restore NuGet Packages
```powershell
nuget restore WindowsUnifiedConnector.sln
```

### Step 7: Build in Visual Studio
```powershell
# Open solution
"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" WindowsUnifiedConnector.sln
```

Or via command line:
```powershell
# Set up build environment
& "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

# Build solution
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m
```

### Expected Output
```
C:\Workspace\WindowsUnifiedConnector\x64\Release\
├── UCService.exe
├── WinPackageManager.exe
├── UCDiagTool.exe
├── ucidapi.dll
├── libLogger.lib
└── [many other binaries]
```

---

## Option B: Conan-Based Build (POC)

### Overview
This is the **new build method** using Conan for public dependencies and mocks for proprietary components. **No VPN required!**

### Prerequisites
- ✅ Python 3.8+ installed
- ✅ Internet access (for downloading Conan packages)

### Step 1: Clone Repository
```bash
cd C:\Workspace
git clone https://github.com/adjamwal/WindowsUnifiedConnector.git
cd WindowsUnifiedConnector
```

### Step 2: Install Conan
```powershell
# Install Conan 2.x
pip install conan

# Verify installation
conan --version
# Should show: Conan version 2.x.x
```

### Step 3: Detect Conan Profile
```powershell
# Auto-detect your build environment
conan profile detect --force

# View generated profile
conan profile show default
```

**Expected output**:
```
[settings]
arch=x86_64
build_type=Release
compiler=msvc
compiler.cppstd=14
compiler.runtime=dynamic
compiler.version=193
os=Windows
```

### Step 4: Install Dependencies with Conan
```powershell
# Install all dependencies from conanfile.txt
conan install . --build=missing -s build_type=Release -s arch=x86_64

# This downloads and builds:
# - jsoncpp/1.9.5
# - fmt/10.2.1
# - spdlog/1.12.0
# - openssl/1.1.1w
# - zlib/1.3
# - libcurl/8.4.0
# - gtest/1.14.0
```

**What this creates**:
- `conaninfo.txt` - Dependency info
- `conanbuildinfo.*` - Build integration files
- `build/generators/` - MSBuild integration files

**Time**: First run takes ~10-15 minutes (builds OpenSSL from source). Subsequent runs use cache (~1 minute).

### Step 5: Build Mock Libraries
```powershell
# Build our custom mocks first
msbuild MockLibraries\MockUCID\MockUCID.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild MockLibraries\MockCommon\MockCommon.vcxproj /p:Configuration=Release /p:Platform=x64
```

**Output**:
```
x64\Release\
├── ucidapi.dll      # Mock UCID
└── lib\
    └── common.lib   # Mock Common
```

### Step 6: Set Environment Variables
```powershell
# For Conan-based builds
$env:USE_CONAN_DEPS = "true"
$env:UC_CONSUMER = "0"
$env:UCID_BUILD_NUMBER = "1.2.18-mock"
```

### Step 7: Restore NuGet Packages
```powershell
nuget restore WindowsUnifiedConnector.sln
```

### Step 8: Build Selected Projects
Since not all projects work with mocks yet, start with core components:

```powershell
# Build SharedLibs
msbuild SharedLibs\libLogger\libLogger.vcxproj /p:Configuration=Release /p:Platform=x64 /p:UseConanDeps=true
msbuild SharedLibs\libWindowsUtilities\libWindowsUtilities.vcxproj /p:Configuration=Release /p:Platform=x64

# Build UC Components
msbuild UC\libUcConfig\libUcConfig.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild UC\UCService\UCService.vcxproj /p:Configuration=Release /p:Platform=x64
```

### Current Status (Conan Build)
⚠️ **Note**: Conan-based build is a work-in-progress POC. Not all 50+ projects build yet.

**Working**:
- ✅ Conan dependency installation
- ✅ Mock library builds
- 🔄 Some SharedLibs (libLogger, etc.)

**Known Issues**:
- Projects still reference `$(IMN_COMMON)` paths
- Need to update .vcxproj files for Conan integration
- Some components depend on proprietary libs

---

## Building the Project

### Build Configurations

The solution supports 4 configurations:
```
Debug|x64       # Debug build, 64-bit
Release|x64     # Release build, 64-bit
Debug|x86       # Debug build, 32-bit
Release|x86     # Release build, 32-bit
```

### Build from Visual Studio

**Method 1: Visual Studio GUI**
1. Open `WindowsUnifiedConnector.sln`
2. Select configuration: `Release` + `x64`
3. Menu → **Build** → **Build Solution** (Ctrl+Shift+B)

**Method 2: Developer Command Prompt**
```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
cd C:\Workspace\WindowsUnifiedConnector
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m
```

**Method 3: PowerShell**
```powershell
# Import MSBuild
$msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"

# Build solution
& $msbuild WindowsUnifiedConnector.sln `
    /p:Configuration=Release `
    /p:Platform=x64 `
    /maxcpucount `
    /verbosity:minimal
```

### Build Specific Projects

**Individual project**:
```powershell
msbuild UC\UCService\UCService.vcxproj /p:Configuration=Release /p:Platform=x64
```

**Multiple projects**:
```powershell
$projects = @(
    "SharedLibs\libLogger\libLogger.vcxproj",
    "UC\libUcConfig\libUcConfig.vcxproj",
    "UC\UCService\UCService.vcxproj"
)

foreach ($proj in $projects) {
    Write-Host "Building $proj..."
    msbuild $proj /p:Configuration=Release /p:Platform=x64
}
```

### Build Installer (WiX)

**Prerequisites**: WiX Toolset 3.11+ installed

```powershell
# Build main installer
msbuild Install\Installer\Installer.wixproj /p:Configuration=Release /p:Platform=x64

# Output: Install\bin\x64\Release\WindowsUnifiedConnector.msi
```

---

## Project Structure

```
WindowsUnifiedConnector/
├── .github/workflows/          # GitHub Actions CI/CD
├── UC/                         # Core UC components
│   ├── UCService/              # Main service
│   ├── libUcConfig/            # Configuration library
│   ├── libUcLoader/            # Loader library
│   └── UCDiagTool/             # Diagnostic tool
├── SharedLibs/                 # Shared libraries
│   ├── libLogger/              # Logging library
│   ├── libWindowsUtilities/    # Windows utilities
│   ├── libProxy/               # Proxy support
│   └── MockWindowsUtilities/   # Mock utilities (for testing)
├── PackageManager/             # Package manager (submodule)
│   ├── libPmClient/
│   ├── libPmUtil/
│   └── libPmCloud/
├── WindowsPackageManager/      # Windows PM
│   ├── WinPackageManager/
│   ├── libPmAgent/
│   └── dllControlModule/
├── UCID/                       # Identity module
│   ├── UCID.vcxproj
│   ├── FakeCMIDApiDll/         # Fake UCID for testing
│   └── UpdateUCID.ps1          # Download script
├── Install/                    # Installer projects
│   ├── Installer/              # Main MSI
│   ├── Bootstrapper/           # Bootstrapper
│   └── InstallerCustomActions/
├── MockLibraries/              # POC mock libraries
│   ├── MockUCID/               # Mock UCID (for Conan builds)
│   ├── MockCommon/             # Mock common.lib
│   └── MockSigning/            # Mock signing
├── Resources/                  # Resource files
│   ├── config/                 # Configuration files
│   └── images/                 # Icons
├── conanfile.txt              # Conan dependencies (new)
├── build.spec                 # AmpRepositorySync config (old)
├── Directory.Build.props      # MSBuild properties
└── WindowsUnifiedConnector.sln # Visual Studio solution
```

---

## Build Output Locations

### Traditional Build
```
WindowsUnifiedConnector/
├── x64/Release/               # 64-bit Release builds
│   ├── UCService.exe
│   ├── WinPackageManager.exe
│   ├── ucidapi.dll
│   └── *.lib files
├── x64/Debug/                 # 64-bit Debug builds
├── x86/Release/               # 32-bit Release builds
└── x86/Debug/                 # 32-bit Debug builds
```

### Conan Build
```
WindowsUnifiedConnector/
├── x64/Release/               # Main binaries
├── build/                     # Conan build artifacts
│   └── generators/
│       ├── conanbuildinfo.props
│       └── conan_toolchain.props
└── MockLibraries/
    ├── MockUCID/x64/Release/
    └── MockCommon/x64/Release/
```

---

## Troubleshooting

### Issue 1: "Cannot open include file: 'xxx.h'"

**Symptom**:
```
fatal error C1083: Cannot open include file: 'json/json.h': No such file or directory
```

**Cause**: Missing dependencies or incorrect paths

**Solution (Traditional Build)**:
```powershell
# Ensure dependencies are fetched
AmpRepositorySync.exe checkout build.spec

# Verify common directory exists
dir ..\common\common-windows-build\Release\x64\include
```

**Solution (Conan Build)**:
```powershell
# Reinstall Conan dependencies
conan install . --build=missing -s build_type=Release -s arch=x86_64

# Check generated files
dir build\generators
```

---

### Issue 2: "Cannot open input file 'common.lib'"

**Symptom**:
```
LINK : fatal error LNK1181: cannot open input file 'common.lib'
```

**Cause**: Missing common library

**Solution (Traditional Build)**:
- Check `$(IMN_COMMON)\common-windows-build\Release\x64\lib\common.lib` exists
- Verify AmpRepositorySync completed successfully

**Solution (Conan Build)**:
```powershell
# Build MockCommon
msbuild MockLibraries\MockCommon\MockCommon.vcxproj /p:Configuration=Release /p:Platform=x64

# Verify output
dir x64\Release\lib\common.lib
```

---

### Issue 3: "UCID_Initialize undefined"

**Symptom**:
```
error LNK2019: unresolved external symbol UCID_Initialize
```

**Cause**: Missing UCID library

**Solution (Traditional Build)**:
```powershell
# Download UCID
cd UCID
.\UpdateUCID.ps1

# Verify files
dir Resources\UCID\ucidapi.dll
```

**Solution (Conan Build)**:
```powershell
# Build MockUCID
msbuild MockLibraries\MockUCID\MockUCID.vcxproj /p:Configuration=Release /p:Platform=x64

# Verify output
dir x64\Release\ucidapi.dll
```

---

### Issue 4: MSBuild Not Found

**Symptom**:
```
'msbuild' is not recognized as an internal or external command
```

**Solution**:
```powershell
# Option 1: Use Developer Command Prompt
"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"

# Option 2: Add MSBuild to PATH
$env:PATH += ";C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin"

# Option 3: Use full path
& "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" WindowsUnifiedConnector.sln
```

---

### Issue 5: Conan Profile Error

**Symptom**:
```
ERROR: Conan profile not found
```

**Solution**:
```powershell
# Regenerate profile
conan profile detect --force

# Verify profile
conan profile show default

# If issues persist, create manually
conan profile path default
# Edit the file and set compiler=msvc, compiler.version=193
```

---

### Issue 6: NuGet Package Restore Fails

**Symptom**:
```
Error: Unable to find version 'x.x.x' of package 'PackageName'
```

**Solution**:
```powershell
# Clear NuGet cache
nuget locals all -clear

# Restore with verbose output
nuget restore WindowsUnifiedConnector.sln -Verbosity detailed

# Check NuGet.Config
cat NuGet.Config
```

---

### Issue 7: Wrong Visual Studio Version

**Symptom**:
```
error MSB8020: The build tools for v142 cannot be found
```

**Solution**:
- Install Visual Studio 2022 (v143 toolset)
- Or update project files to use installed toolset
- Check `Directory.Build.props` for `<PlatformToolset>v143</PlatformToolset>`

---

## Build Performance Tips

### 1. Use Parallel Builds
```powershell
# Use all CPU cores
msbuild WindowsUnifiedConnector.sln /maxcpucount
```

### 2. Build Only What Changed
```powershell
# Incremental build (default)
msbuild WindowsUnifiedConnector.sln
```

### 3. Reduce Verbosity
```powershell
# Minimal output
msbuild WindowsUnifiedConnector.sln /verbosity:minimal
```

### 4. Cache Conan Packages
```powershell
# Conan automatically caches in: ~/.conan2/
# Reuse across projects - no need to rebuild OpenSSL every time!
```

### 5. Use SSD for Build
- Place workspace on SSD, not HDD
- Significantly faster I/O

---

## Typical Build Times

| Configuration | Method | Time (Clean) | Time (Incremental) |
|---------------|--------|--------------|-------------------|
| Release x64 | Traditional (with cache) | ~5-10 min | ~1-2 min |
| Release x64 | Conan (first time) | ~15-20 min | ~2-3 min |
| Release x64 | Conan (cached) | ~5-8 min | ~1-2 min |
| Single project | Any | ~30 sec - 2 min | ~10-30 sec |
| Installer MSI | WiX | ~2-3 min | ~1 min |

**Note**: Times vary based on hardware (CPU, RAM, SSD).

---

## Verification Steps

### Verify Successful Build

**Check binaries exist**:
```powershell
# Main executables
Test-Path x64\Release\UCService.exe
Test-Path x64\Release\WinPackageManager.exe
Test-Path x64\Release\UCDiagTool.exe

# Libraries
Test-Path x64\Release\lib\libLogger.lib
Test-Path x64\Release\ucidapi.dll
```

**Run smoke test**:
```powershell
# Check executable runs
.\x64\Release\UCService.exe --help
.\x64\Release\UCDiagTool.exe --version
```

### Run Unit Tests

**Using Visual Studio**:
1. Menu → **Test** → **Run All Tests**
2. View results in Test Explorer

**Using vstest.console**:
```powershell
# Find test assemblies
$tests = Get-ChildItem -Path x64\Release -Filter "Test*.exe" -Recurse

# Run tests
foreach ($test in $tests) {
    & "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe" $test.FullName
}
```

---

## Quick Start Cheat Sheet

### Traditional Build (with VPN)
```powershell
# 1. Clone
git clone https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector.git
cd WindowsUnifiedConnector

# 2. Get dependencies
AmpRepositorySync.exe checkout build.spec
cd UCID && .\UpdateUCID.ps1 && cd ..

# 3. Restore packages
nuget restore WindowsUnifiedConnector.sln

# 4. Build
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m
```

### Conan Build (no VPN)
```powershell
# 1. Clone
git clone https://github.com/adjamwal/WindowsUnifiedConnector.git
cd WindowsUnifiedConnector

# 2. Install Conan
pip install conan
conan profile detect --force

# 3. Get dependencies
conan install . --build=missing -s build_type=Release -s arch=x86_64

# 4. Build mocks
msbuild MockLibraries\MockUCID\MockUCID.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild MockLibraries\MockCommon\MockCommon.vcxproj /p:Configuration=Release /p:Platform=x64

# 5. Restore NuGet
nuget restore WindowsUnifiedConnector.sln

# 6. Build selected projects
msbuild SharedLibs\libLogger\libLogger.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild UC\UCService\UCService.vcxproj /p:Configuration=Release /p:Platform=x64
```

---

## Additional Resources

### Documentation
- **BUILD_DEPENDENCIES.md** - Complete dependency list
- **CONAN_MIGRATION_ANALYSIS.md** - Conan migration strategy
- **GITHUB_ACTIONS_GUIDE.md** - CI/CD setup guide
- **BUILD_PHASES.md** - Phased build strategy

### External Links
- Visual Studio: https://visualstudio.microsoft.com/
- Conan: https://docs.conan.io/
- WiX Toolset: https://wixtoolset.org/
- MSBuild Reference: https://docs.microsoft.com/en-us/visualstudio/msbuild/

### Internal Links (Cisco VPN required)
- Jenkins CI: https://clg5-lab-winjenkins.cisco.com/view/UnifiedConnector/job/UnifiedConnector/job/WindowsUnifiedConnector/
- Source Repo: https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector
- UCID: https://code.engine.sourcefire.com/UnifiedConnector/EndpointIdentity

---

## Support

### For Traditional Build Issues
- Check Jenkins build logs for successful builds
- Verify VPN connection to Cisco network
- Contact team for AmpRepositorySync access

### For Conan Build Issues
- Check GitHub Actions logs: https://github.com/adjamwal/WindowsUnifiedConnector/actions
- Review CONAN_MIGRATION_ANALYSIS.md for known limitations
- Note: Conan build is POC, not all features work yet

---

**Last Updated**: January 19, 2026  
**Build Method**: Traditional (Production) + Conan (POC)  
**Recommended**: Use Traditional build for production, Conan for POC/testing
