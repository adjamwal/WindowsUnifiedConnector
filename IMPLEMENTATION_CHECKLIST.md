# Implementation Checklist: GitHub Actions CI/CD Setup

## Quick Start Summary

**Goal**: Build Windows Unified Connector via GitHub Actions using Conan for dependency management.

**Approach**: Hybrid strategy - Use Conan for public libraries, create mocks for Cisco proprietary components.

**Timeline**: 4-5 days for minimal POC, 2-3 weeks for complete setup.

---

## Phase 1: Initial Setup (Day 1) ✓ PRIORITY

### ☐ 1.1 Create Conan Configuration

```bash
cd /Users/adjamwal/WindowsUnifiedConnector
```

Create `conanfile.txt`:
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

**Test locally**:
```powershell
pip install conan
conan profile detect --force
conan install . --build=missing -s build_type=Release -s arch=x86_64
```

### ☐ 1.2 Create Directory Structure

```powershell
# Create mock library directories
New-Item -ItemType Directory -Force -Path MockLibraries/MockUCID
New-Item -ItemType Directory -Force -Path MockLibraries/MockCommon
New-Item -ItemType Directory -Force -Path MockLibraries/MockSigning
New-Item -ItemType Directory -Force -Path scripts
New-Item -ItemType Directory -Force -Path .github/workflows
```

### ☐ 1.3 Update .gitignore

Add to `.gitignore`:
```
# Conan
conan_cache/
conaninfo.txt
conanbuildinfo.*
graph_info.json
build/generators/

# Mock binaries
MockLibraries/*/bin/
MockLibraries/*/lib/
```

---

## Phase 2: Mock Libraries (Day 1-2) ✓ CRITICAL

### ☐ 2.1 Create Mock UCID

Files to create:
- `MockLibraries/MockUCID/ucidapi.h` (see GITHUB_ACTIONS_GUIDE.md Step 5)
- `MockLibraries/MockUCID/mock_ucidapi.cpp`
- `MockLibraries/MockUCID/MockUCID.vcxproj`

**MockUCID.vcxproj template**:
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{AAAAAAAA-BBBB-CCCC-DDDD-EEEEEEEEEEEE}</ProjectGuid>
    <ConfigurationType>DynamicLibrary</ConfigurationType>
    <PlatformToolset>v143</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
    <OutDir>$(SolutionDir)$(Platform)\$(Configuration)\</OutDir>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ItemGroup>
    <ClCompile Include="mock_ucidapi.cpp" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="ucidapi.h" />
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
```

### ☐ 2.2 Create Mock Common Library

Files to create:
- `MockLibraries/MockCommon/common.h`
- `MockLibraries/MockCommon/mock_common.cpp`
- `MockLibraries/MockCommon/MockCommon.vcxproj` (similar structure to MockUCID)

### ☐ 2.3 Create Mock Signing Script

`MockLibraries/MockSigning/mock_sign.bat`:
```batch
@echo off
echo [MOCK] Skipping signature for POC: %1
exit /b 0
```

---

## Phase 3: Build Configuration (Day 2-3)

### ☐ 3.1 Create Modified Directory.Build.props

Create `Directory.Build.Conan.props`:
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Label="UserMacros">
    <WLHBASE>C:\Program Files (x86)\Windows Kits\10</WLHBASE>
    <UC_PATH>$([System.IO.Path]::GetFullPath('$(MSBuildThisFileDirectory)'))</UC_PATH>
    
    <!-- Use Conan dependencies instead of IMN_COMMON -->
    <UseConanDeps Condition="'$(UseConanDeps)' == ''">true</UseConanDeps>
    <IMN_COMMON Condition="'$(UseConanDeps)' == 'true'">$(UC_PATH)build\</IMN_COMMON>
    <IMN_COMMON Condition="'$(UseConanDeps)' != 'true'">$([System.IO.Path]::GetFullPath('$(UC_PATH)..\common\'))</IMN_COMMON>
    
    <VC_VARS_PATH>C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build</VC_VARS_PATH>
    <VCToolsVersion>14.34.31933</VCToolsVersion>
    <WindowsTargetPlatformVersion>10.0.22621.0</WindowsTargetPlatformVersion>
    <UC_CONSUMER>0</UC_CONSUMER>
    <UCID_BUILD_NUMBER>1.2.18-mock</UCID_BUILD_NUMBER>
  </PropertyGroup>
</Project>
```

### ☐ 3.2 Test Local Build with Conan

```powershell
# Install Conan dependencies
conan install . --build=missing -s build_type=Release -s arch=x86_64

# Build mock libraries first
msbuild MockLibraries/MockUCID/MockUCID.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild MockLibraries/MockCommon/MockCommon.vcxproj /p:Configuration=Release /p:Platform=x64

# Try building one component (e.g., UCService)
msbuild UC/UCService/UCService.vcxproj /p:Configuration=Release /p:Platform=x64 /p:UseConanDeps=true
```

---

## Phase 4: GitHub Actions Workflows (Day 3-4)

### ☐ 4.1 Create Basic Build Workflow

Create `.github/workflows/build.yml` (see GITHUB_ACTIONS_GUIDE.md Step 8)

**Minimal version for testing**:
```yaml
name: Build POC

on:
  push:
    branches: [ master, main ]
  workflow_dispatch:

jobs:
  build:
    runs-on: windows-2022
    steps:
    - uses: actions/checkout@v4
    
    - name: Setup Python
      uses: actions/setup-python@v5
      with:
        python-version: '3.11'
    
    - name: Install Conan
      run: pip install conan
    
    - name: Setup Conan
      run: |
        conan profile detect --force
        conan install . --build=missing -s build_type=Release -s arch=x86_64
    
    - name: Setup MSBuild
      uses: microsoft/setup-msbuild@v1.3
    
    - name: Build Mock Libraries
      run: |
        msbuild MockLibraries/MockUCID/MockUCID.vcxproj /p:Configuration=Release /p:Platform=x64
        msbuild MockLibraries/MockCommon/MockCommon.vcxproj /p:Configuration=Release /p:Platform=x64
    
    - name: Build UCService
      run: |
        msbuild UC/UCService/UCService.vcxproj /p:Configuration=Release /p:Platform=x64 /p:UseConanDeps=true
```

### ☐ 4.2 Push and Test

```bash
git add .
git commit -m "Add GitHub Actions CI with Conan support"
git push origin master
```

Monitor: https://github.com/adjamwal/WindowsUnifiedConnector/actions

---

## Phase 5: Iterative Fixes (Day 4-5)

### ☐ 5.1 Fix Include Path Issues

If you see: `fatal error C1083: Cannot open include file: 'json/json.h'`

**Solution**: Update .vcxproj files to include Conan paths:
```xml
<AdditionalIncludeDirectories>
  $(ConanIncludeDirs);
  $(UC_PATH)UC\libUcConfig;
  %(AdditionalIncludeDirectories)
</AdditionalIncludeDirectories>
```

### ☐ 5.2 Fix Linker Issues

If you see: `LINK : fatal error LNK1181: cannot open input file`

**Solution**: Update library directories:
```xml
<AdditionalLibraryDirectories>
  $(ConanLibDirs);
  $(OutDir);
  %(AdditionalLibraryDirectories)
</AdditionalLibraryDirectories>
```

### ☐ 5.3 Add More Mock Implementations

As you encounter missing functions:
1. Note the function signature from error
2. Add stub to appropriate mock library
3. Rebuild and test

---

## Phase 6: Expand Coverage (Week 2)

### ☐ 6.1 Add More Components to Build

Priority order:
1. ✅ UCService (main service)
2. ☐ libUcConfig
3. ☐ libUcLoader
4. ☐ WindowsPackageManager
5. ☐ Test projects

### ☐ 6.2 Enable Testing in CI

```yaml
- name: Run Unit Tests
  run: |
    $tests = Get-ChildItem -Path "x64/Release" -Filter "Test*.exe"
    foreach ($test in $tests) {
      & $test.FullName --gtest_output=xml:test-results.xml
    }
```

### ☐ 6.3 Add Matrix Builds

```yaml
strategy:
  matrix:
    platform: [x64, Win32]
    configuration: [Debug, Release]
```

---

## Phase 7: Installer Build (Week 3)

### ☐ 7.1 Install WiX in Workflow

```yaml
- name: Install WiX
  run: choco install wixtoolset -y
```

### ☐ 7.2 Build MSI Installer

```yaml
- name: Build Installer
  run: |
    msbuild Install/Installer/Installer.wixproj /p:Configuration=Release /p:Platform=x64
```

---

## Testing Checklist

### Local Testing

- [ ] Conan installs dependencies successfully
- [ ] Mock libraries compile without errors
- [ ] UCService compiles with mocked dependencies
- [ ] Can run resulting executable (even if functionality is limited)

### CI Testing

- [ ] GitHub Actions workflow triggers on push
- [ ] Conan setup completes successfully
- [ ] Mock libraries build in CI
- [ ] Main components build in CI
- [ ] Artifacts are uploaded

### Integration Testing

- [ ] Built binaries are correct architecture (x64/x86)
- [ ] DLL dependencies are resolved
- [ ] Basic smoke test passes

---

## Common Issues & Solutions

### Issue: Conan Profile Error
```
ERROR: Conan profile not found
```
**Fix**: Run `conan profile detect --force` in workflow

### Issue: MSBuild Not Found
```
'msbuild' is not recognized
```
**Fix**: Add `microsoft/setup-msbuild@v1.3` action

### Issue: Linking Errors with Conan Libs
```
LNK2019: unresolved external symbol
```
**Fix**: 
- Check library names match (jsoncpp vs jsoncpp.lib)
- Verify MSBuildDeps generator is used
- Import conan_toolchain.props in vcxproj

### Issue: Git Submodule Errors
```
fatal: No url found for submodule
```
**Fix**: Update .gitmodules or use `submodules: false` in checkout action

---

## Success Criteria

### Minimal POC (Day 5)
- [x] Repository setup complete
- [ ] Conan dependencies install in CI
- [ ] Mock libraries build successfully
- [ ] At least one core component (UCService) builds
- [ ] Build artifacts uploaded

### Complete Setup (Week 3)
- [ ] All major components build
- [ ] Unit tests run in CI
- [ ] x64 and x86 builds work
- [ ] MSI installer generated
- [ ] Documentation complete

---

## Quick Commands Reference

### Local Development
```powershell
# Setup Conan
conan install . --build=missing -s build_type=Release -s arch=x86_64

# Build everything
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /p:UseConanDeps=true

# Run tests
.\scripts\run_tests.ps1 -Configuration Debug -Platform x64
```

### CI Debugging
```bash
# View workflow runs
open https://github.com/adjamwal/WindowsUnifiedConnector/actions

# Download artifacts
gh run download <run-id>

# Re-run failed job
gh run rerun <run-id>
```

---

## Next Actions (Prioritized)

1. **TODAY**: Create conanfile.txt and test locally
2. **TODAY**: Create MockUCID and MockCommon projects
3. **TODAY**: Push to GitHub and create basic workflow
4. **TOMORROW**: Fix build errors iteratively
5. **THIS WEEK**: Expand to all core components
6. **NEXT WEEK**: Add installer build

---

## Resources

- 📄 `CONAN_MIGRATION_ANALYSIS.md` - Detailed dependency analysis
- 📄 `GITHUB_ACTIONS_GUIDE.md` - Complete workflow documentation
- 📄 `BUILD_DEPENDENCIES.md` - Original dependency list
- 🔗 [Conan Docs](https://docs.conan.io/)
- 🔗 [GitHub Actions Docs](https://docs.github.com/en/actions)

---

**Status**: Ready to implement  
**Next Step**: Create conanfile.txt and mock libraries
