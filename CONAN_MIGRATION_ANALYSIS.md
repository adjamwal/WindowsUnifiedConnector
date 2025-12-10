# Conan Migration Analysis for Windows Unified Connector

## Executive Summary

The WindowsUnifiedConnector project currently uses a **custom dependency management system** (AmpRepositorySync) that fetches prebuilt libraries from Cisco internal repositories. This analysis evaluates the feasibility of migrating to **Conan** for dependency management to enable building via GitHub Actions.

**Result**: **Hybrid Approach Required** - Approximately 60% of dependencies can be migrated to Conan, but Cisco-proprietary components require alternative solutions.

---

## Current Dependency Management Architecture

### AmpRepositorySync System
- **Tool**: AmpRepositorySync.exe (Cisco proprietary)
- **Source**: Cisco internal Nexus repositories
- **Storage**: `../common/` directory (relative to solution)
- **Configuration**: `build.spec` file
- **Path References**: All projects reference `$(IMN_COMMON)common-windows-build\`

### Dependency Categories

#### Category 1: Public/Open-Source Libraries (Conan Compatible ✅)
These libraries are available in Conan Center and can be migrated:

| Library | Current Source | Conan Package | Conan Version | Notes |
|---------|---------------|---------------|---------------|-------|
| **GoogleTest/GoogleMock** | fireamp-win-google-test Build 28 | gtest | 1.14.0+ | Fully compatible |
| **Jsoncpp** | fireamp-win-jsoncpp Build 43 | jsoncpp | 1.9.5+ | Widely used |
| **Curl** | curl-rel Build 69 | libcurl | 8.x+ | Standard package |
| **lib-fmt** | fireamp-win-lib-fmt Build 10 | fmt | 10.x+ | Active in Conan |
| **spdlog** | spdlog-rel Build 12 | spdlog | 1.x+ | Has fmt dependency |
| **Zlib** | (via common-windows-build) | zlib | 1.3.x | Standard package |
| **OpenSSL** | (libcrypto-1_1.dll, libssl-1_1.dll) | openssl | 3.x or 1.1.x | OpenSSL 1.1 branch |
| **libxml2** | libxml2-rel Build 11 | libxml2 | 2.x+ | Available |
| **Expat** | fireamp-win-expat Build 25 | expat | 2.x+ | XML parser |
| **RE2** | fireamp-win-re2 Build 11 | re2 | 2023+ | Regex library |

**Estimated Coverage**: ~50-60% of third-party dependencies

#### Category 2: Cisco Proprietary/Internal Libraries (❌ Not Available)
These components are Cisco-specific and not publicly available:

| Component | Purpose | Alternative for POC |
|-----------|---------|---------------------|
| **common.lib** | Cisco common utilities | Mock/stub implementation |
| **common-legacy** | Legacy Cisco code | Exclude or mock |
| **UCID** (v1.2.18) | Unified Connector Identity | Mock UCID API or use test doubles |
| **fireamp-win-clamav** | ClamAV integration | Use public ClamAV or mock |
| **fireamp-win-est** | EST protocol | Mock implementation |
| **fireamp-win-safec** | Safe C library | Use Microsoft SafeC or alternative |
| **fireamp-win-xml-signer** | XML signing | Mock for POC |
| **fireamp-win-xmlsec** | XML security | Use public xmlsec1 |
| **fireamp-win-namedpipe** | Named pipe utilities | Mock or reimplement |
| **fireamp-win-httpmockserver** | HTTP mock server | Use cpp-httplib or similar |

#### Category 3: Crash Reporting Libraries (⚠️ Partially Available)

| Library | Status | Solution |
|---------|--------|----------|
| **Google Breakpad** | Available in some repos | Build from source or use sentry-native |
| **Google Crashpad** | Public repo available | Build from Chromium source or use alternative |

---

## Conan Migration Feasibility

### ✅ Can Migrate to Conan (High Priority)

1. **GoogleTest/GoogleMock** - Critical for testing
   ```python
   gtest/1.14.0
   ```

2. **Jsoncpp** - JSON parsing
   ```python
   jsoncpp/1.9.5
   ```

3. **libcurl** - HTTP client
   ```python
   libcurl/8.4.0
   ```

4. **fmt** - String formatting
   ```python
   fmt/10.1.1
   ```

5. **spdlog** - Logging
   ```python
   spdlog/1.12.0
   ```

6. **OpenSSL** - Crypto/SSL
   ```python
   openssl/1.1.1w  # Match current version
   ```

7. **zlib** - Compression
   ```python
   zlib/1.3
   ```

### ⚠️ Requires Custom Handling

1. **UCID Module** (v1.2.18)
   - **Current**: Downloaded from `nexus.engine.sourcefire.com` via PowerShell script
   - **Options**:
     - Create mock UCID implementation for POC
     - Store UCID binaries in GitHub repo (if licensing allows)
     - Use GitHub Secrets to store Nexus credentials
     - Create stub DLLs with minimal API

2. **Cisco Common Libraries**
   - **Options**:
     - Extract minimal required functionality
     - Create header-only stubs
     - Mock implementations for build purposes

3. **Code Signing**
   - **Current**: Uses `signingvs10_noph.bat` and Cisco certificates
   - **Solution**: Skip signing for POC builds or self-sign

---

## Blockers for Full GitHub Actions CI

### Critical Blockers

1. **Cisco Internal Repositories Access**
   - AmpRepositorySync requires VPN/network access
   - Nexus repositories are behind Cisco firewall
   - **Impact**: Cannot fetch proprietary dependencies

2. **UCID Dependency**
   - Core identity module (1.2.18) hosted on Cisco Nexus
   - Required by multiple components
   - **Impact**: Build will fail without UCID

3. **Signing Infrastructure**
   - Code signing requires Cisco certificates
   - Signing scripts expect specific paths
   - **Impact**: Cannot produce signed binaries (acceptable for POC)

4. **Windows SDK & Build Tools**
   - Requires specific versions (10.0.22621.0, VS2022 v143)
   - **Impact**: Need properly configured GitHub-hosted runner or self-hosted

### Minor Blockers

1. **WiX Toolset**
   - Need WiX 3.11+ for installer builds
   - **Solution**: Install via Chocolatey in GitHub Actions

2. **Git Submodules**
   - PackageManager submodule references Cisco repo
   - **Solution**: Fork to personal GitHub or mock

---

## Recommended Approach for GitHub Actions POC

### Strategy: Hybrid Conan + Mocked Dependencies

#### Phase 1: Setup Conan Infrastructure (Week 1)

1. **Create `conanfile.py`**
   ```python
   from conan import ConanFile
   from conan.tools.cmake import CMake, cmake_layout
   
   class WindowsUnifiedConnectorConan(ConanFile):
       name = "WindowsUnifiedConnector"
       version = "1.0.4"
       
       settings = "os", "compiler", "build_type", "arch"
       
       def requirements(self):
           self.requires("gtest/1.14.0")
           self.requires("jsoncpp/1.9.5")
           self.requires("libcurl/8.4.0")
           self.requires("fmt/10.1.1")
           self.requires("spdlog/1.12.0")
           self.requires("openssl/1.1.1w")
           self.requires("zlib/1.3")
   ```

2. **Update Directory.Build.props**
   - Replace `$(IMN_COMMON)` paths with Conan-generated paths
   - Use Conan generators (MSBuildDeps, MSBuildToolchain)

#### Phase 2: Create Mock Libraries (Week 1-2)

1. **Mock UCID**
   - Create `MockUCID` project with stub API
   - Implement minimal interface for compilation
   - Store mock binaries in `Resources/MockUCID/`

2. **Mock Common Libraries**
   - Create `MockCommon` project with essential functions
   - Header-only stubs where possible

3. **Mock Signing**
   - Create `mock_sign.bat` that does nothing or self-signs
   - Update post-build events

#### Phase 3: Modify Build Configuration (Week 2)

1. **Create Alternative .props File**
   - `Directory.Build.Conan.props` for Conan builds
   - Conditionally import based on environment variable

2. **Update .vcxproj Files**
   - Programmatically replace library references
   - Use MSBuild conditions for Conan vs. legacy paths

#### Phase 4: GitHub Actions Workflow (Week 2-3)

Create `.github/workflows/build.yml` (see separate file)

---

## Conan Package Mapping

### Detailed Library Mapping

| Current Library | Current Path | Conan Package | Include Path | Link Library |
|-----------------|--------------|---------------|--------------|--------------|
| gtest.lib | $(IMN_COMMON)\..\lib | gtest/1.14.0 | gtest/gtest.h | gtest.lib |
| jsoncpp.lib | $(IMN_COMMON)\..\lib | jsoncpp/1.9.5 | json/json.h | jsoncpp.lib |
| common.lib | $(IMN_COMMON)\..\lib | **MOCK** | mock/common.h | mock_common.lib |
| crash_generation_client.lib | $(IMN_COMMON)\..\lib | **BUILD** | client/crash_generation_client.h | crash_generation_client.lib |
| fmt.lib / fmtd.lib | $(IMN_COMMON)\..\lib | fmt/10.1.1 | fmt/format.h | fmt.lib |
| libcrypto-1_1.dll | $(IMN_COMMON)\..\bin | openssl/1.1.1w | openssl/crypto.h | libcrypto.lib |

---

## File Structure Changes Required

### New Files to Create

```
WindowsUnifiedConnector/
├── conanfile.py                    # Conan package definition
├── conanfile.txt                   # Alternative simpler format
├── CMakeLists.txt                  # Optional: CMake wrapper
├── .github/
│   └── workflows/
│       ├── build.yml               # Main build workflow
│       ├── test.yml                # Test workflow
│       └── release.yml             # Release workflow
├── MockLibraries/                  # Mock implementations
│   ├── MockUCID/
│   │   ├── ucidapi.h
│   │   ├── mock_ucidapi.cpp
│   │   └── MockUCID.vcxproj
│   ├── MockCommon/
│   │   ├── common.h
│   │   ├── mock_common.cpp
│   │   └── MockCommon.vcxproj
│   └── MockSigning/
│       └── mock_sign.bat
├── scripts/
│   ├── setup_conan.ps1             # Setup Conan environment
│   ├── convert_paths.ps1           # Convert IMN_COMMON to Conan paths
│   └── build_with_conan.ps1        # Build script
└── Directory.Build.Conan.props     # Conan-specific build properties
```

### Modified Files

1. **Directory.Build.props** - Add Conan support conditionally
2. **All .vcxproj files** - Update AdditionalIncludeDirectories and AdditionalLibraryDirectories
3. **sign.bat** - Add mock signing option
4. **.gitignore** - Add Conan-generated files

---

## Build Configuration Options

### Option A: Full Conan (Recommended for POC)

**Pros**:
- Standard dependency management
- Reproducible builds
- Easy GitHub Actions integration
- Cross-platform potential

**Cons**:
- Requires mocking Cisco components
- Significant upfront work
- May differ from production build

### Option B: Hybrid with Git LFS

**Pros**:
- Can include some prebuilt binaries
- Faster initial setup
- Closer to production

**Cons**:
- Large repo size
- Binary tracking issues
- Licensing concerns

### Option C: Self-Hosted Runner with VPN

**Pros**:
- Can use existing dependency system
- Minimal code changes
- Production-like environment

**Cons**:
- Infrastructure overhead
- VPN/network requirements
- Security considerations

---

## Effort Estimation

### Conan Migration Effort

| Task | Effort | Dependencies |
|------|--------|--------------|
| Create conanfile.py | 1 day | None |
| Setup Conan in CI | 1 day | conanfile.py |
| Mock UCID implementation | 3-5 days | API documentation |
| Mock Common libraries | 3-5 days | Header analysis |
| Update .vcxproj files | 2-3 days | Understanding all projects |
| Modify Directory.Build.props | 1 day | Conan paths |
| Create GitHub Actions workflow | 2 days | All above |
| Testing and debugging | 3-5 days | Full workflow |
| **Total** | **16-24 days** | N/A |

### Quick POC Approach (Minimal)

| Task | Effort |
|------|--------|
| Minimal conanfile.txt | 2 hours |
| Simple GitHub Actions | 1 day |
| Mock only critical libs | 2 days |
| Test one configuration | 1 day |
| **Total** | **4-5 days** |

---

## Recommendations

### For POC/Demo (Recommended)

1. ✅ **Use Conan** for public dependencies (gtest, jsoncpp, curl, fmt, spdlog, openssl, zlib)
2. ✅ **Create minimal mocks** for UCID and common.lib
3. ✅ **Skip code signing** in POC builds
4. ✅ **Build only core components** (UCService, limited tests)
5. ✅ **Use GitHub-hosted Windows runner** (windows-2022)

### For Production Build

1. 🔄 **Keep AmpRepositorySync** for Cisco dependencies
2. 🔄 **Use self-hosted runner** with VPN access
3. 🔄 **Maintain code signing** with proper certificates
4. 🔄 **Full regression testing** before migration

---

## Next Steps

1. **Create conanfile.py** with public dependencies
2. **Set up minimal mock libraries** for UCID and common
3. **Create GitHub Actions workflow** with Conan setup
4. **Test build on GitHub-hosted runner**
5. **Iterate and expand** coverage

See `GITHUB_ACTIONS_GUIDE.md` for detailed workflow implementation.

---

**Analysis Date**: December 2024  
**Recommendation**: Proceed with Hybrid Conan approach for POC
