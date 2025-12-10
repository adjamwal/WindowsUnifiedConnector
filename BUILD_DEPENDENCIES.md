# Windows Unified Connector - Build Dependencies & Requirements

## Overview
This document provides a comprehensive list of all dependencies, third-party libraries, tools, and requirements needed to build the Windows Unified Connector project.

---

## Build Environment Requirements

### Operating System
- **Windows 10/11** (Build 22621.0 or later recommended)
- Windows SDK: **10.0.22621.0**

### Development Tools

#### Visual Studio 2022
- **Version**: Professional or Enterprise
- **Platform Toolset**: v143
- **VC Tools Version**: 14.34.31933
- **VC Redistributable Version**: 14.34.31931
- **Location**: `C:\Program Files\Microsoft Visual Studio\2022\Professional`
- **Components Required**:
  - C++ Desktop Development
  - Windows 10/11 SDK (10.0.22621.0)
  - C++17 Standard Support
  - MSBuild
  - Visual Studio Test Adapter (vstest.console)

#### WiX Toolset
- **Version**: 3.11 or newer
- **Purpose**: Building Windows installers (.msi and .exe)
- **Download**: https://wixtoolset.org/releases/
- **Extensions Required**:
  - WixUtilExtension
- **Note**: Visual Studio extensions should be installed for integration

#### NuGet Package Manager
- **Version**: Latest
- **Download**: https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
- **Purpose**: Restore project packages

---

## Third-Party Libraries & Dependencies

### Core Dependencies (from build.spec)

These dependencies are fetched via AmpRepositorySync and stored in the `common` directory:

| Library/Component | Version/Build |
|-------------------|---------------|
| **fireamp-win-clamav** | Build 102 |
| **common-legacy** | Build 109 |
| **fireamp-win-common-nexus** | Build 123 |
| **curl-rel** | Build 69 |
| **fireamp-win-est** | Build 21 |
| **fireamp-win-expat** | Build 25 |
| **fireamp-win-google-breakpad** | Latest |
| **fireamp-win-google-crashpad** | Build 49 |
| **fireamp-win-google-test** | Build 28 |
| **fireamp-win-httpmockserver** | Build 22 |
| **fireamp-win-jsoncpp** | Build 43 |
| **fireamp-win-lib-fmt** | Build 10 |
| **libxml2-rel** | Build 11 |
| **fireamp-win-namedpipe** | Build 27 |
| **fireamp-win-re2** | Build 11 |
| **fireamp-win-safec** | Build 18 |
| **fireamp-win-xml-signer** | Latest |
| **fireamp-win-xmlsec** | Build 20 |
| **spdlog-rel** | Build 12 |

### Major Third-Party Libraries (as mentioned in README.md)

1. **UCID (Unified Connector Identity)**
   - Version: 1.2.18 (from Directory.Build.props)
   - Repository: https://code.engine.sourcefire.com/UnifiedConnector/EndpointIdentity
   - Purpose: Endpoint identity management

2. **CiscoSSL**
   - Purpose: SSL/TLS communications

3. **Curl**
   - Purpose: HTTP/HTTPS client communications
   - Version: From curl-rel Build 69

4. **FakeFunctionFramework**
   - Purpose: Testing framework for mocking

5. **GoogleTest/GoogleMock**
   - Version: Build 28
   - Purpose: Unit testing framework
   - NuGet Package: GoogleTestAdapter 0.18.0

6. **Jsoncpp**
   - Version: Build 43
   - Purpose: JSON parsing and generation
   - Linked as: jsoncpp.lib

7. **Zlib**
   - Purpose: Compression library

8. **Google Breakpad**
   - Version: Latest
   - Purpose: Crash reporting
   - Linked libraries: crash_generation_client.lib, exception_handler.lib

9. **Google Crashpad**
   - Version: Build 49
   - Purpose: Crash reporting (newer alternative to Breakpad)

10. **lib-fmt**
    - Version: Build 10
    - Purpose: String formatting library
    - Linked as: fmt.lib (Release), fmtd.lib (Debug)

11. **spdlog**
    - Version: Build 12
    - Purpose: Fast C++ logging library

### Windows System Libraries

The following Windows SDK libraries are required:

- **Winhttp.lib** - HTTP client
- **Msi.lib** - Windows Installer API
- **Wintrust.lib** - Certificate/signing verification
- **Crypt32.lib** - Cryptography API
- **kernel32.lib** - Core Windows API
- **user32.lib** - User interface
- **gdi32.lib** - Graphics device interface
- **winspool.lib** - Print spooler
- **comdlg32.lib** - Common dialogs
- **advapi32.lib** - Advanced Windows API (Registry, Services)
- **shell32.lib** - Shell API
- **ole32.lib** - OLE/COM
- **oleaut32.lib** - OLE Automation
- **uuid.lib** - GUID support
- **odbc32.lib** - Database connectivity
- **odbccp32.lib** - ODBC control panel

---

## NuGet Packages

| Package | Version | Purpose |
|---------|---------|---------|
| **GoogleTestAdapter** | 0.18.0 | Visual Studio Test Adapter for Google Test |

**NuGet Source**: https://api.nuget.org/v3/index.json

---

## Git Submodules

| Submodule | Path | Repository |
|-----------|------|------------|
| **PackageManager** | ./PackageManager | https://code.engine.sourcefire.com/UnifiedConnector/PackageManager.git |

---

## Build Configuration

### Language Standard
- **C++ Standard**: C++17 (ISO/IEC 14882:2017)
- **Character Set**: Unicode

### Runtime Library
- **Debug**: MultiThreadedDebugDLL (/MDd)
- **Release**: MultiThreadedDLL (/MD)

### Compiler Settings
- Warning Level: Level3
- Treat Warnings as Errors: Yes
- Optimization (Release): MaxSpeed (/O2)
- Intrinsic Functions: Yes (Release)

### Platform Targets
- **x86** (Win32)
- **x64**

### Build Configurations
- **Debug**
- **Release**

---

## Code Signing Requirements

### Signing Tool
- Tool: signingvs10_noph.bat
- Location: `%IMN_COMMON%\common-windows-build\Tools\signing\signingvs10_noph.bat`
- Uses: Microsoft SignTool
- Timestamp: Enabled (USE_SIGNING_TIMESTAMP=1)

### Certificates
- Local Certificate Location: `C:\machineCerts`
- Certificate File: MyCA.cer

### Signed Artifacts
- All executables (.exe)
- All DLLs
- All MSI installers

---

## Build Process Overview

### 1. Checkout Dependencies
```bash
# Uses AmpRepositorySync.exe to fetch dependencies
AmpRepositorySync.exe -cws .
AmpRepositorySync.exe -cc WindowsUnifiedConnector\build.spec
```

### 2. Initialize Git Submodules
```bash
git submodule update --init --recursive
```

### 3. Restore NuGet Packages
```bash
nuget.exe restore WindowsUnifiedConnector.sln
```

### 4. Build Solution
```bash
# Setup Visual Studio environment
"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"

# Build x86 Release
msbuild.exe WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x86 -m

# Build x64 Release
msbuild.exe WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 -m

# Build x86 Debug
msbuild.exe WindowsUnifiedConnector.sln /p:Configuration=Debug /p:Platform=x86 -m

# Build x64 Debug
msbuild.exe WindowsUnifiedConnector.sln /p:Configuration=Debug /p:Platform=x64 -m
```

### 5. Run Tests
```bash
# Set PATH to include common build binaries
set PATH=%WORKSPACE%\common\common-windows-build\Release\Win32\bin;%PATH%

# Run Debug x86 tests
vstest.console Win32\Debug\Test*.exe /logger:trx /Platform:x86 /TestAdapterPath:packages\GoogleTestAdapter.0.18.0\build\_common

# Run Debug x64 tests
vstest.console x64\Debug\Test*.exe /logger:trx /Platform:x64 /TestAdapterPath:packages\GoogleTestAdapter.0.18.0\build\_common

# Run Component Tests
vstest.console Win32\Debug\ComponentTest*.exe /logger:trx /Platform:x86 /TestAdapterPath:packages\GoogleTestAdapter.0.18.0\build\_common
vstest.console x64\Debug\ComponentTest*.exe /logger:trx /Platform:x64 /TestAdapterPath:packages\GoogleTestAdapter.0.18.0\build\_common
```

---

## Project Structure

### Main Components

1. **UC (Unified Connector Service)**
   - UCService (csc_cms.exe) - Main service executable
   - UCDiagTool - Diagnostic tool
   - libUcConfig - Configuration library
   - libUcLoader - Module loader
   - libUcDiagTool - Diagnostic library

2. **UCID (Unified Connector Identity)**
   - UCID.dll - Identity module
   - FakeCMIDApiDll - Test/mock API
   - FakeUCB - Fake backend for testing

3. **WindowsPackageManager**
   - WinPackageManager - Package management executable
   - dllControlModule - Control module for service
   - libPmAgent - Package manager agent library
   - libControlModule - Control module library

4. **SharedLibs**
   - libLogger / libLoggerMT - Logging libraries
   - libProxy (MT/MD) - Proxy handling
   - libWindowsUtilities - Windows utility functions
   - libToast - Toast notification support
   - libCrashHandlerClient - Crash reporting client
   - libVerifier - Verification utilities

5. **Install**
   - Installer (WiX) - MSI installer project
   - Bootstrapper (WiX) - Bundle/bootstrapper
   - InstallerCustomActions - Custom installer actions
   - dllCaSupport - Custom action support library

---

## Environment Variables

Required environment variables (set by Directory.Build.props):

| Variable | Default Value | Purpose |
|----------|---------------|---------|
| **WLHBASE** | C:\Program Files (x86)\Windows Kits\10 | Windows SDK location |
| **UC_PATH** | Project root directory | Unified Connector path |
| **IMN_COMMON** | ../common/ | Common dependencies path |
| **VC_VARS_PATH** | C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build | Visual Studio tools path |
| **UC_CONSUMER** | 0 | Consumer mode flag |
| **UCID_BUILD_NUMBER** | 1.2.18 | UCID version |
| **VCToolsVersion** | 14.34.31933 | VC++ toolset version |
| **WindowsTargetPlatformVersion** | 10.0.22621.0 | Windows SDK version |

---

## Known External Dependencies (Not in Repository)

### From Sourcefire/Cisco Repositories
These components need to be obtained from internal Cisco/Sourcefire repositories:

1. **AmpRepositorySync.exe** - Dependency synchronization tool
2. **Common build tools** - Located in fireamp-win-client-build-scripts
3. **Signing tools** - signingvs10_noph.bat
4. **UCID module** - From EndpointIdentity repository

### Network Shares (for CI/CD)
- **Build Location**: `\\\\clg5-lab-fserv1.cisco.com\\public\\WinUCBuild\\Automation`

---

## CI/CD Information

### Jenkins Build Server
- **URL**: https://clg5-lab-winjenkins.cisco.com/view/UnifiedConnector/job/UnifiedConnector/job/WindowsUnifiedConnector/
- **Build Agents**: windowsci2019, branchBuildEnabled2019

### Version Information
- **Major**: 1
- **Minor**: 0
- **Patch**: 4
- **Format**: {Major}.{Minor}.{Patch}.{BuildNumber}

---

## Additional Resources

### Documentation
- **SecureX/Device Insights**: https://securex.test.iroh.site/
- **Catalog Repository**: https://code.engine.sourcefire.com/UnifiedConnector/identity-catalog
- **Wiki**: https://wiki.cisco.com/display/THEEND/Unified+Connector+-+Identity+Module

### Related Repositories
- **Main Repository**: https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector
- **EndpointIdentity**: https://code.engine.sourcefire.com/UnifiedConnector/EndpointIdentity
- **PackageManager**: https://code.engine.sourcefire.com/UnifiedConnector/PackageManager
- **Identity Catalog**: https://code.engine.sourcefire.com/UnifiedConnector/identity-catalog

---

## Summary Checklist

To successfully build this project, ensure you have:

- [ ] Windows 10/11 with latest updates
- [ ] Visual Studio 2022 Professional/Enterprise with C++ Desktop Development
- [ ] Windows SDK 10.0.22621.0
- [ ] WiX Toolset 3.11 or newer
- [ ] NuGet CLI tool
- [ ] Git with submodule support
- [ ] Access to Sourcefire/Cisco internal repositories
- [ ] AmpRepositorySync.exe tool
- [ ] Code signing certificates (for production builds)
- [ ] Network access to dependency repositories
- [ ] All dependencies from build.spec fetched into ../common/ directory
- [ ] PackageManager submodule initialized
- [ ] UCID version 1.2.18 installed

---

**Generated**: December 2024  
**Project Version**: 1.0.4 (v1.0.4.447)
