# GitHub Actions Build Guide for Windows Unified Connector

## Overview

This guide provides step-by-step instructions for setting up GitHub Actions CI/CD for the Windows Unified Connector project using a hybrid Conan + Mock dependencies approach.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Project Structure Setup](#project-structure-setup)
3. [Conan Configuration](#conan-configuration)
4. [Mock Libraries Creation](#mock-libraries-creation)
5. [GitHub Actions Workflows](#github-actions-workflows)
6. [Build Process](#build-process)
7. [Testing Strategy](#testing-strategy)
8. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### GitHub Repository Setup

- ✅ Repository: `adjamwal/WindowsUnifiedConnector` (already done)
- ✅ Code migrated from Sourcefire (already done)
- ⬜ Enable GitHub Actions
- ⬜ Configure repository secrets (if needed)

### Local Development Setup (Optional)

```powershell
# Install Conan 2.x
pip install conan

# Verify installation
conan --version

# Configure Conan profile
conan profile detect --force
```

---

## Project Structure Setup

### Step 1: Create Required Directories

```powershell
cd /Users/adjamwal/WindowsUnifiedConnector

# Create directories
New-Item -ItemType Directory -Force -Path .github/workflows
New-Item -ItemType Directory -Force -Path MockLibraries/MockUCID
New-Item -ItemType Directory -Force -Path MockLibraries/MockCommon
New-Item -ItemType Directory -Force -Path MockLibraries/MockSigning
New-Item -ItemType Directory -Force -Path scripts
```

### Step 2: Update .gitignore

Add Conan and build artifacts:

```gitignore
# Conan
conan_cache/
conaninfo.txt
conanbuildinfo.*
graph_info.json
CMakeUserPresets.json

# Build artifacts
build/
cmake-build-*/

# Mock dependencies (if prebuilt)
MockLibraries/*/bin/
MockLibraries/*/lib/
```

---

## Conan Configuration

### Step 3: Create conanfile.py

Create a comprehensive Conan recipe:

```python
from conan import ConanFile
from conan.tools.microsoft import MSBuild, MSBuildDeps, MSBuildToolchain
from conan.tools.files import copy
import os

class WindowsUnifiedConnectorConan(ConanFile):
    name = "WindowsUnifiedConnector"
    version = "1.0.4"
    license = "Proprietary"
    description = "Windows Unified Connector - Cloud Management Service"
    
    settings = "os", "compiler", "build_type", "arch"
    
    options = {
        "shared": [True, False],
        "with_tests": [True, False],
    }
    
    default_options = {
        "shared": False,
        "with_tests": True,
    }
    
    def requirements(self):
        # Core dependencies
        self.requires("jsoncpp/1.9.5")
        self.requires("fmt/10.1.1")
        self.requires("spdlog/1.12.0")
        self.requires("openssl/1.1.1w")  # Match current 1.1.x version
        self.requires("zlib/1.3")
        self.requires("libcurl/8.4.0")
        
        # Test dependencies
        if self.options.with_tests:
            self.requires("gtest/1.14.0")
    
    def build_requirements(self):
        # Build tools if needed
        pass
    
    def configure(self):
        # Configuration logic
        if self.settings.os != "Windows":
            raise ConanInvalidConfiguration("This package only supports Windows")
    
    def layout(self):
        # Define layout for build artifacts
        self.folders.build = "build"
        self.folders.generators = "build/generators"
    
    def generate(self):
        # Generate MSBuild integration files
        deps = MSBuildDeps(self)
        deps.generate()
        
        tc = MSBuildToolchain(self)
        tc.generate()
    
    def build(self):
        # Build using MSBuild
        msbuild = MSBuild(self)
        # We'll trigger this from GitHub Actions instead
        # msbuild.build("WindowsUnifiedConnector.sln")
    
    def package(self):
        # Package the built artifacts
        copy(self, "*.exe", src=self.build_folder, dst=os.path.join(self.package_folder, "bin"), keep_path=False)
        copy(self, "*.dll", src=self.build_folder, dst=os.path.join(self.package_folder, "bin"), keep_path=False)
    
    def package_info(self):
        self.cpp_info.bindirs = ["bin"]
```

### Step 4: Create conanfile.txt (Simpler Alternative)

For a simpler setup:

```ini
[requires]
jsoncpp/1.9.5
fmt/10.1.1
spdlog/1.12.0
openssl/1.1.1w
zlib/1.3
libcurl/8.4.0
gtest/1.14.0

[generators]
MSBuildDeps
MSBuildToolchain

[options]
shared=False
```

---

## Mock Libraries Creation

### Step 5: Create Mock UCID

Create `MockLibraries/MockUCID/ucidapi.h`:

```cpp
#pragma once
#ifndef MOCK_UCIDAPI_H
#define MOCK_UCIDAPI_H

#ifdef __cplusplus
extern "C" {
#endif

// Mock UCID API - Minimal interface for compilation
typedef void* UCID_HANDLE;

// Mock function declarations
__declspec(dllexport) int UCID_Initialize(UCID_HANDLE* handle);
__declspec(dllexport) int UCID_GetIdentity(UCID_HANDLE handle, char* buffer, int size);
__declspec(dllexport) int UCID_GetSessionToken(UCID_HANDLE handle, char* buffer, int size);
__declspec(dllexport) void UCID_Cleanup(UCID_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif // MOCK_UCIDAPI_H
```

Create `MockLibraries/MockUCID/mock_ucidapi.cpp`:

```cpp
#include "ucidapi.h"
#include <string.h>

// Mock implementations
int UCID_Initialize(UCID_HANDLE* handle) {
    // Return success for POC
    *handle = (UCID_HANDLE)0x12345678;
    return 0;
}

int UCID_GetIdentity(UCID_HANDLE handle, char* buffer, int size) {
    const char* mockId = "mock-identity-12345";
    strncpy_s(buffer, size, mockId, _TRUNCATE);
    return 0;
}

int UCID_GetSessionToken(UCID_HANDLE handle, char* buffer, int size) {
    const char* mockToken = "mock-token-abcdef";
    strncpy_s(buffer, size, mockToken, _TRUNCATE);
    return 0;
}

void UCID_Cleanup(UCID_HANDLE handle) {
    // Nothing to clean up in mock
}
```

### Step 6: Create Mock Common Library

Create `MockLibraries/MockCommon/common.h`:

```cpp
#pragma once
#ifndef MOCK_COMMON_H
#define MOCK_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

// Mock common library functions
__declspec(dllexport) int Common_Init();
__declspec(dllexport) void Common_Log(const char* message);
__declspec(dllexport) int Common_GetVersion(char* buffer, int size);

#ifdef __cplusplus
}
#endif

#endif // MOCK_COMMON_H
```

Create `MockLibraries/MockCommon/mock_common.cpp`:

```cpp
#include "common.h"
#include <stdio.h>
#include <string.h>

int Common_Init() {
    return 0;  // Success
}

void Common_Log(const char* message) {
    printf("[MOCK_COMMON] %s\n", message);
}

int Common_GetVersion(char* buffer, int size) {
    const char* version = "1.0.0-mock";
    strncpy_s(buffer, size, version, _TRUNCATE);
    return 0;
}
```

### Step 7: Create Mock Signing Script

Create `MockLibraries/MockSigning/mock_sign.bat`:

```batch
@echo off
REM Mock signing script for POC builds
REM Simply returns success without actual signing

echo [MOCK SIGN] Skipping code signing for: %1
echo [MOCK SIGN] POC build - no signature applied
exit /b 0
```

---

## GitHub Actions Workflows

### Step 8: Main Build Workflow

Create `.github/workflows/build.yml`:

```yaml
name: Build Windows Unified Connector

on:
  push:
    branches: [ master, main, develop ]
  pull_request:
    branches: [ master, main ]
  workflow_dispatch:

env:
  SOLUTION_FILE: WindowsUnifiedConnector.sln
  BUILD_CONFIGURATION: Release
  BUILD_PLATFORM: x64

jobs:
  build:
    name: Build ${{ matrix.platform }}-${{ matrix.configuration }}
    runs-on: windows-2022
    
    strategy:
      matrix:
        platform: [x64, Win32]
        configuration: [Debug, Release]
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v4
      with:
        submodules: recursive
        fetch-depth: 0
    
    - name: Setup Python
      uses: actions/setup-python@v5
      with:
        python-version: '3.11'
    
    - name: Install Conan
      run: |
        pip install conan
        conan --version
    
    - name: Detect Conan Profile
      run: |
        conan profile detect --force
    
    - name: Setup Conan Profile for MSVC
      run: |
        conan profile show default
        # Ensure we're using MSVC settings
        conan profile update settings.compiler="Visual Studio" default
        conan profile update settings.compiler.version=193 default
    
    - name: Install Dependencies with Conan
      run: |
        # Install dependencies
        conan install . --build=missing -s build_type=${{ matrix.configuration }} -s arch=${{ matrix.platform == 'Win32' && 'x86' || 'x86_64' }}
    
    - name: Setup MSBuild
      uses: microsoft/setup-msbuild@v1.3
      with:
        vs-version: '17.0'
    
    - name: Setup NuGet
      uses: NuGet/setup-nuget@v1
    
    - name: Restore NuGet Packages
      run: nuget restore ${{ env.SOLUTION_FILE }}
    
    - name: Build Mock Libraries
      run: |
        # Build mock UCID
        msbuild MockLibraries/MockUCID/MockUCID.vcxproj /p:Configuration=${{ matrix.configuration }} /p:Platform=${{ matrix.platform }}
        
        # Build mock common
        msbuild MockLibraries/MockCommon/MockCommon.vcxproj /p:Configuration=${{ matrix.configuration }} /p:Platform=${{ matrix.platform }}
    
    - name: Setup Mock Signing
      run: |
        # Replace sign.bat with mock version
        Copy-Item MockLibraries/MockSigning/mock_sign.bat sign.bat -Force
    
    - name: Set Environment Variables
      run: |
        # Set IMN_COMMON to mock location for POC
        echo "IMN_COMMON=${{ github.workspace }}/build" >> $GITHUB_ENV
        echo "UC_CONSUMER=0" >> $GITHUB_ENV
        echo "UCID_BUILD_NUMBER=1.2.18-mock" >> $GITHUB_ENV
    
    - name: Build Solution
      run: |
        msbuild ${{ env.SOLUTION_FILE }} `
          /p:Configuration=${{ matrix.configuration }} `
          /p:Platform=${{ matrix.platform }} `
          /p:UseConanDeps=true `
          /maxcpucount `
          /verbosity:minimal
    
    - name: Run Tests
      if: matrix.configuration == 'Debug'
      run: |
        # Find and run test executables
        $testExes = Get-ChildItem -Path "${{ matrix.platform }}/${{ matrix.configuration }}" -Filter "Test*.exe" -Recurse
        foreach ($test in $testExes) {
          Write-Host "Running: $($test.FullName)"
          & $test.FullName --gtest_output=xml:test-results-$($test.BaseName).xml
        }
      continue-on-error: true
    
    - name: Upload Test Results
      if: matrix.configuration == 'Debug'
      uses: actions/upload-artifact@v4
      with:
        name: test-results-${{ matrix.platform }}-${{ matrix.configuration }}
        path: test-results-*.xml
    
    - name: Upload Binaries
      uses: actions/upload-artifact@v4
      with:
        name: binaries-${{ matrix.platform }}-${{ matrix.configuration }}
        path: |
          ${{ matrix.platform }}/${{ matrix.configuration }}/*.exe
          ${{ matrix.platform }}/${{ matrix.configuration }}/*.dll
          !${{ matrix.platform }}/${{ matrix.configuration }}/*.pdb
        retention-days: 30
    
    - name: Upload Build Logs
      if: failure()
      uses: actions/upload-artifact@v4
      with:
        name: build-logs-${{ matrix.platform }}-${{ matrix.configuration }}
        path: |
          **/*.log
          **/msbuild.log
        retention-days: 7
```

### Step 9: Installer Build Workflow

Create `.github/workflows/build-installer.yml`:

```yaml
name: Build Installer

on:
  push:
    tags:
      - 'v*'
  workflow_dispatch:

jobs:
  build-installer:
    name: Build MSI Installer
    runs-on: windows-2022
    needs: [] # Can depend on build job if you want
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v4
      with:
        submodules: recursive
    
    - name: Install WiX Toolset
      run: |
        choco install wixtoolset -y
        # Add WiX to PATH
        echo "C:\Program Files (x86)\WiX Toolset v3.11\bin" | Out-File -FilePath $env:GITHUB_PATH -Encoding utf8 -Append
    
    - name: Setup MSBuild
      uses: microsoft/setup-msbuild@v1.3
    
    - name: Build Installer
      run: |
        # Build the WiX installer project
        msbuild Install/Installer/Installer.wixproj `
          /p:Configuration=Release `
          /p:Platform=x64 `
          /p:OutputPath=bin/x64/Release/
    
    - name: Upload Installer
      uses: actions/upload-artifact@v4
      with:
        name: installer-msi
        path: Install/Installer/bin/x64/Release/*.msi
```

### Step 10: Pull Request Checks Workflow

Create `.github/workflows/pr-checks.yml`:

```yaml
name: Pull Request Checks

on:
  pull_request:
    branches: [ master, main ]

jobs:
  quick-build:
    name: Quick Build Check (x64 Debug)
    runs-on: windows-2022
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Setup MSBuild
      uses: microsoft/setup-msbuild@v1.3
    
    - name: Quick Compile Check
      run: |
        # Quick sanity build
        msbuild WindowsUnifiedConnector.sln /p:Configuration=Debug /p:Platform=x64 /t:Build /maxcpucount
  
  code-analysis:
    name: Static Code Analysis
    runs-on: windows-2022
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Run Code Analysis
      run: |
        # Add static analysis tools here (e.g., clang-tidy, cppcheck)
        echo "Code analysis placeholder"
```

---

## Build Process

### Step 11: Setup Scripts

Create `scripts/setup_conan.ps1`:

```powershell
# Setup Conan environment for Windows Unified Connector
param(
    [string]$BuildType = "Release",
    [string]$Arch = "x86_64"
)

Write-Host "Setting up Conan environment..."
Write-Host "Build Type: $BuildType"
Write-Host "Architecture: $Arch"

# Check if Conan is installed
if (!(Get-Command conan -ErrorAction SilentlyContinue)) {
    Write-Error "Conan is not installed. Please install with: pip install conan"
    exit 1
}

# Detect and configure profile
conan profile detect --force

# Install dependencies
Write-Host "Installing dependencies..."
conan install . --build=missing -s build_type=$BuildType -s arch=$Arch

Write-Host "Conan setup complete!"
```

Create `scripts/build_with_conan.ps1`:

```powershell
# Build Windows Unified Connector with Conan dependencies
param(
    [string]$Configuration = "Release",
    [string]$Platform = "x64"
)

# Setup Conan
& "$PSScriptRoot\setup_conan.ps1" -BuildType $Configuration -Arch ($Platform -eq "Win32" ? "x86" : "x86_64")

# Setup Visual Studio environment
$vsPath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath
Import-Module "$vsPath\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell -VsInstallPath $vsPath -SkipAutomaticLocation

# Build solution
Write-Host "Building solution..."
msbuild WindowsUnifiedConnector.sln `
    /p:Configuration=$Configuration `
    /p:Platform=$Platform `
    /maxcpucount `
    /verbosity:normal

Write-Host "Build complete!"
```

---

## Testing Strategy

### Step 12: Configure Test Execution

Create `scripts/run_tests.ps1`:

```powershell
# Run all tests in the solution
param(
    [string]$Configuration = "Debug",
    [string]$Platform = "x64"
)

$testPath = "$Platform\$Configuration"
$testExecutables = Get-ChildItem -Path $testPath -Filter "Test*.exe" -Recurse

$failedTests = 0
$passedTests = 0

foreach ($test in $testExecutables) {
    Write-Host "`nRunning: $($test.Name)" -ForegroundColor Cyan
    
    $result = & $test.FullName --gtest_output=xml:test-results-$($test.BaseName).xml
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ PASSED" -ForegroundColor Green
        $passedTests++
    } else {
        Write-Host "✗ FAILED" -ForegroundColor Red
        $failedTests++
    }
}

Write-Host "`nTest Summary:" -ForegroundColor Yellow
Write-Host "  Passed: $passedTests"
Write-Host "  Failed: $failedTests"

exit $failedTests
```

---

## Troubleshooting

### Common Issues

#### Issue 1: Conan Dependencies Not Found

**Error**: `LINK : fatal error LNK1181: cannot open input file 'jsoncpp.lib'`

**Solution**:
```powershell
# Ensure Conan generated MSBuild files
conan install . --build=missing

# Check generated files
ls build/generators/
```

#### Issue 2: UCID Missing

**Error**: `Cannot find UCID module`

**Solution**:
- Ensure MockUCID is built first
- Check that mock binaries are in correct location
- Verify Directory.Build.props points to mock path

#### Issue 3: Signing Failures

**Error**: `signingvs10_noph.bat failed`

**Solution**:
- Replace sign.bat with mock_sign.bat
- Or modify post-build events to skip signing

#### Issue 4: Windows SDK Version Mismatch

**Error**: `Windows SDK version 10.0.22621.0 not found`

**Solution**:
```yaml
# Update Directory.Build.props or use GitHub Actions SDK setup
- name: Setup Windows SDK
  run: |
    choco install windows-sdk-10-version-2004-all
```

### Debug Mode

Enable verbose logging:

```yaml
- name: Build with Verbose Logging
  run: |
    msbuild ${{ env.SOLUTION_FILE }} `
      /p:Configuration=Debug `
      /p:Platform=x64 `
      /verbosity:detailed `
      /fl /flp:logfile=msbuild.log
```

---

## Next Steps

1. ✅ Commit conanfile.py and workflows to repository
2. ⬜ Push to GitHub and verify Actions run
3. ⬜ Fix any build errors iteratively
4. ⬜ Add more mock implementations as needed
5. ⬜ Expand test coverage
6. ⬜ Set up release automation

---

## Additional Resources

- [Conan Documentation](https://docs.conan.io/)
- [GitHub Actions - Windows](https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners#supported-runners-and-hardware-resources)
- [MSBuild Reference](https://docs.microsoft.com/en-us/visualstudio/msbuild/msbuild-reference)
- [WiX Toolset](https://wixtoolset.org/documentation/)

---

**Last Updated**: December 2024  
**Status**: Ready for Implementation
