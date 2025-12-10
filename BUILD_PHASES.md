# End-to-End Build Strategy: Phased Approach

## Current Solution Analysis

**Total Projects**: 50+ projects in WindowsUnifiedConnector.sln

### Project Categories

#### Core Runtime Components (8 projects) 🎯 **PRIORITY**
1. **UCService** - Main service executable
2. **libUcConfig** - Configuration library
3. **libUcLoader** - Loader library
4. **libUcDiagTool** - Diagnostic tool library
5. **UCDiagTool** - Diagnostic tool executable
6. **UCID** - Identity module (requires mock)
7. **dllUiVersionPlugin** - Version plugin
8. **FakeCMIDApiDll** - Fake UCID API (already a mock!)

#### Shared Libraries (12 projects)
- libLogger, libLoggerMT
- libWindowsUtilities
- libVerifier
- libProxyMD, libProxyMT
- libToast
- libCrashHandlerClient
- ComponentTestVerifier
- ProxyTestTool

#### Package Manager (11 projects)
- libPmClient, libPmUtil, libPmCloud, libPmConfig
- WindowsPackageManager components
- libPmAgent, libControlModule, dllControlModule
- WinPackageManager

#### Windows Package Manager (5 projects)
- libPmAgent
- libControlModule
- dllControlModule
- WinPackageManager
- WinPackageManagerVerifier

#### Test Projects (15+ projects)
- TestUcConfig, TestLibUCDiagTool
- TestPackageManagerClient, TestPmUtil
- ComponentTest* projects
- Test* projects

#### Mock Projects (Already in solution! 10 projects)
- MockWindowsUtilities
- MockVerifier
- MockLibUCDiagTool
- MockPmCloud, MockPmClient, MockPmUtil
- MockPmConfig, MockPmInterface
- MockPmAgent
- MockProxy

#### Installer Projects (4 projects) - **REQUIRES WIX**
- Installer (MSI)
- Bootstrapper
- InstallerCustomActions
- dllCaSupport

---

## Phased Build Strategy

### 🟢 Phase 1: Foundation (CURRENT - In Progress)
**Goal**: Get Conan working + Build mock libraries + Build 1-2 core components

**Status**: ✅ Conan working, ⏳ Mock libraries building

#### Projects to Build
1. ✅ MockUCID (our custom mock)
2. ✅ MockCommon (our custom mock)
3. ⏳ libLogger
4. ⏳ UCService (attempting)

#### Blockers
- Path references still point to `$(IMN_COMMON)`
- Projects need to find Conan-generated files
- Mock libraries need correct output paths

**Timeline**: 1-2 days

---

### 🟡 Phase 2: Core Components (Next Week)
**Goal**: Build all core UC components without tests

#### Projects to Build (Priority Order)
1. **SharedLibs**
   - libLogger ✅
   - libLoggerMT
   - libWindowsUtilities
   - libCrashHandlerClient
   - libVerifier

2. **UC Core**
   - libUcConfig
   - libUcLoader
   - UCService
   - libUcDiagTool
   - UCDiagTool

3. **UCID** (Use existing FakeCMIDApiDll or our MockUCID)
   - FakeCMIDApiDll (already in solution!)
   - FakeUCB

4. **Proxy**
   - libProxyMD
   - libProxyMT

#### Required Updates
- Update all .vcxproj files to reference Conan include paths
- Update all .vcxproj files to reference Conan library paths
- Use existing mock projects from solution
- Set up proper build order (dependencies)

**Timeline**: 1 week

---

### 🟡 Phase 3: Package Manager Components (Week 2-3)
**Goal**: Build Package Manager and Windows Package Manager

#### Projects to Build
1. **Package Manager Base**
   - libPmUtil
   - libPmConfig
   - libPmClient
   - libPmCloud

2. **Windows Package Manager**
   - libPmAgent
   - libControlModule
   - dllControlModule
   - WinPackageManager

3. **Utilities**
   - libToast
   - dllUiVersionPlugin

#### Challenges
- PackageManager is a git submodule (may be empty/broken)
- These depend on UCID heavily
- Complex interdependencies

**Timeline**: 1-2 weeks

---

### 🟠 Phase 4: Test Projects (Week 3-4)
**Goal**: Enable unit tests in CI

#### Test Projects (15+ total)
- TestUcConfig
- TestLibUCDiagTool
- TestPackageManagerClient
- TestPmUtil
- TestPmConfig
- TestPmAgent
- ComponentTest* projects

#### Requirements
- GoogleTest/GoogleMock from Conan ✅
- Test runners configured
- Mocks properly set up
- Test data files

**Timeline**: 1 week

---

### 🔴 Phase 5: Installer (Week 4-5)
**Goal**: Generate MSI installer

#### Installer Projects
- Installer.wixproj (MSI)
- Bootstrapper.wixproj
- InstallerCustomActions
- dllCaSupport

#### Requirements
- WiX Toolset 3.11+ installed in GitHub Actions
- All binaries built successfully
- Code signing mocked
- Harvest files for installer

**Timeline**: 1 week

---

## Updated GitHub Actions Strategy

### Current Workflow (Phase 1)
```yaml
- Build MockUCID
- Build MockCommon
- Build libLogger (attempting)
- Build UCService (attempting)
```

### Improved Phase 2 Workflow

```yaml
jobs:
  build-foundation:
    - Build our MockUCID
    - Build our MockCommon
    
  build-shared-libs:
    needs: build-foundation
    - Build libLogger
    - Build libLoggerMT
    - Build libWindowsUtilities
    - Build libCrashHandlerClient
    - Build libVerifier
    
  build-uc-core:
    needs: [build-foundation, build-shared-libs]
    - Build FakeCMIDApiDll (solution's mock)
    - Build libUcConfig
    - Build libUcLoader
    - Build UCService
    - Build libUcDiagTool
    - Build UCDiagTool
    
  upload-artifacts:
    - Upload all built binaries
```

### Phase 3+ Workflow

Add these jobs:
- build-package-manager
- build-windows-package-manager
- run-tests
- build-installer

---

## What Can We Build RIGHT NOW?

### ✅ Definitely Can Build
1. **MockUCID** (our custom)
2. **MockCommon** (our custom)
3. **libLogger** (minimal dependencies)
4. **libLoggerMT** (variant of libLogger)

### ⏳ Probably Can Build (with fixes)
1. **libWindowsUtilities** (Windows APIs)
2. **libVerifier** (uses OpenSSL from Conan)
3. **libCrashHandlerClient** (uses Breakpad)
4. **FakeCMIDApiDll** (solution's UCID mock - might work!)

### ❓ Maybe Can Build (more work needed)
1. **UCService** (complex, many dependencies)
2. **libUcConfig** (depends on common.lib)
3. **libProxyMD/MT** (depends on libLogger, curl)

### ❌ Can't Build Yet
1. **Installer projects** (need WiX)
2. **Test projects** (need all libs built first)
3. **Package Manager** (depends on everything)

---

## Recommended Next Actions

### Today - Complete Phase 1 ✅
1. ✅ Fix remaining Conan issues
2. ⏳ Get MockUCID/MockCommon building successfully
3. ⏳ Get libLogger building
4. ⏳ Document current errors

### This Week - Start Phase 2 🎯
1. **Day 1-2**: Fix project file paths
   - Update all AdditionalIncludeDirectories to include Conan paths
   - Update all AdditionalLibraryDirectories to include Conan paths
   
2. **Day 3-4**: Build SharedLibs
   - libLogger ✅
   - libLoggerMT
   - libWindowsUtilities
   - libVerifier
   
3. **Day 5**: Use solution's existing mocks
   - Try building FakeCMIDApiDll instead of our MockUCID
   - Try building existing MockWindowsUtilities
   
4. **Day 6-7**: Build first UC component
   - libUcConfig
   - UCService (attempt)

### Next Week - Complete Phase 2
1. Build all SharedLibs
2. Build all UC core components
3. Create comprehensive artifact uploads
4. Add build matrix (x64 + x86)

---

## Key Insights from Solution Analysis

### Good News 👍
1. **Solution already has mock projects!**
   - MockWindowsUtilities
   - MockVerifier
   - MockPmCloud, MockPmClient, etc.
   - FakeCMIDApiDll (UCID mock!)
   
2. **Test infrastructure exists**
   - 15+ test projects
   - GoogleTest/GoogleMock already used
   
3. **Clear project organization**
   - Separated by functionality
   - Dependencies well-defined

### Challenges 😬
1. **50+ projects** - Can't build all at once
2. **Complex dependencies** - Build order matters
3. **UCID everywhere** - Most components need it
4. **PackageManager submodule** - May be broken
5. **Installer needs WiX** - Additional tool required

### Strategy 💡
1. **Incremental approach** - Phase by phase
2. **Use existing mocks** - Don't reinvent the wheel
3. **Focus on core first** - UC components before tests
4. **Leverage Conan** - For public dependencies
5. **Skip installer initially** - Add in Phase 5

---

## Success Metrics by Phase

### Phase 1 Success (This Week)
- [ ] Conan installs all dependencies ✅ (Done!)
- [ ] MockUCID builds
- [ ] MockCommon builds
- [ ] libLogger builds
- [ ] At least 1 artifact uploaded

### Phase 2 Success (Week 2)
- [ ] All SharedLibs build (8 projects)
- [ ] FakeCMIDApiDll builds
- [ ] libUcConfig builds
- [ ] UCService builds
- [ ] 15+ artifacts uploaded

### Phase 3 Success (Week 3)
- [ ] Package Manager libs build (4 projects)
- [ ] Windows Package Manager builds (4 projects)
- [ ] 30+ artifacts uploaded

### Phase 4 Success (Week 4)
- [ ] 5+ test projects build
- [ ] Tests run in CI
- [ ] Test results published

### Phase 5 Success (Week 5)
- [ ] MSI installer generated
- [ ] Complete end-to-end build
- [ ] All 50+ projects compile

---

## Current GitHub Actions Workflow Status

### What's Running Now
```yaml
✅ Checkout code
✅ Setup Python & Conan
✅ Conan profile detect
✅ Install Conan dependencies (WORKING!)
⏳ Setup MSBuild
⏳ Build MockUCID
⏳ Build MockCommon
⏳ Build libLogger
⏳ Build UCService
```

### What We Need to Add
1. Build order enforcement
2. Use solution's existing mocks
3. Proper artifact organization
4. Build status reporting
5. Incremental build strategy

---

## Blockers to Address

### Critical Blockers (Must Fix for Phase 1)
1. **Include paths** - Projects can't find Conan headers
2. **Library paths** - Linker can't find Conan libraries
3. **Mock output paths** - Mocks need to output where projects expect

### Medium Blockers (Fix for Phase 2)
1. **UCID dependency** - Use FakeCMIDApiDll or our MockUCID
2. **Common.lib dependency** - Use our MockCommon
3. **Build order** - Enforce dependencies
4. **Submodule** - Handle PackageManager

### Low Priority Blockers (Fix for Phase 5)
1. **Code signing** - Already mocked
2. **WiX Toolset** - Install in workflow
3. **Test data files** - Copy to build output

---

## Bottom Line

**Can we build end-to-end NOW?** 
❌ **No** - But we're on track!

**When can we?**
- ✅ **Phase 1 (Foundation)**: This week - 4 projects
- 🎯 **Phase 2 (Core)**: Week 2 - 15 projects
- 🎯 **Phase 3 (Full libs)**: Week 3 - 30 projects
- 🎯 **Phase 4 (Tests)**: Week 4 - 45 projects
- 🎯 **Phase 5 (Complete)**: Week 5 - 50+ projects + MSI

**What's working RIGHT NOW?**
✅ Conan dependency management
✅ GitHub Actions infrastructure
✅ Mock library framework

**Next milestone**: Get 1 complete component building (libLogger + UCService)

---

**Let's focus on Phase 1 completion this week, then expand incrementally!** 🚀
