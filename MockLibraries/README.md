# Mock Libraries

This directory contains mock implementations of Cisco proprietary dependencies for POC builds.

## Purpose

These mock libraries allow the Windows Unified Connector to build in GitHub Actions without access to Cisco internal repositories.

## Contents

### MockUCID
Mock implementation of the Unified Connector Identity (UCID) module.
- **ucidapi.h** - Mock UCID API header
- **mock_ucidapi.cpp** - Stub implementations
- **MockUCID.vcxproj** - Visual Studio project

**Output**: `ucidapi.dll` - Provides basic identity functionality for compilation

### MockCommon
Mock implementation of Cisco common utilities library.
- **common.h** - Mock common library header  
- **mock_common.cpp** - Stub implementations
- **MockCommon.vcxproj** - Visual Studio project

**Output**: `common.lib` - Static library with utility stubs

### MockSigning
Mock code signing script for POC builds.
- **mock_sign.bat** - Bypasses code signing

## Usage

These libraries are automatically built by the GitHub Actions workflow before the main solution.

```yaml
- name: Build Mock Libraries
  run: |
    msbuild MockLibraries/MockUCID/MockUCID.vcxproj
    msbuild MockLibraries/MockCommon/MockCommon.vcxproj
```

## Limitations

⚠️ **These are stubs only** - They provide minimal functionality for compilation.

- No actual identity management
- No real crypto operations
- No network connectivity
- Suitable for POC/demo builds only

## Adding More Mocks

As you encounter more missing dependencies, add mock implementations here:

1. Create header file with API declarations
2. Create .cpp with stub implementations
3. Create .vcxproj for building
4. Add to GitHub Actions workflow

---

**Status**: POC Implementation  
**Production**: Use real UCID from Cisco Nexus
