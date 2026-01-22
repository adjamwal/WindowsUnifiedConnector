# Remote Build Setup: SSH to Windows VM

This guide shows how to set up SSH remote development from your Mac to a Windows VM for building Windows Unified Connector.

---

## Overview

**Workflow**: Edit code on Mac → Build/Run on Windows VM via SSH

**Benefits**:
- ✅ Use your Mac's familiar environment (Windsurf/VS Code)
- ✅ Build happens on Windows VM with proper toolchain
- ✅ No need to RDP or switch machines
- ✅ Git operations on Mac, builds on Windows
- ✅ Full IDE features (IntelliSense, debugging, etc.)

---

## Part 1: Setup SSH Server on Windows VM

### Step 1: Enable OpenSSH Server (Windows 10/11/Server)

**Method 1: Via PowerShell (Admin)**

On your Windows VM, open PowerShell as Administrator:

```powershell
# Check if OpenSSH Server is installed
Get-WindowsCapability -Online | Where-Object Name -like 'OpenSSH.Server*'

# Install OpenSSH Server
Add-WindowsCapability -Online -Name OpenSSH.Server~~~~0.0.1.0

# Start SSH service
Start-Service sshd

# Set to start automatically
Set-Service -Name sshd -StartupType 'Automatic'

# Verify it's running
Get-Service sshd
```

**Method 2: Via Settings GUI**
1. Open **Settings** → **Apps** → **Optional Features**
2. Click **Add a feature**
3. Search for **OpenSSH Server**
4. Install it
5. Go to **Services** (services.msc)
6. Find **OpenSSH SSH Server**
7. Right-click → **Properties** → Set **Startup type** to **Automatic**
8. Click **Start**

### Step 2: Configure Firewall

```powershell
# Allow SSH through Windows Firewall (Port 22)
New-NetFirewallRule -Name sshd -DisplayName 'OpenSSH Server (sshd)' -Enabled True -Direction Inbound -Protocol TCP -Action Allow -LocalPort 22

# Or use existing rule
Get-NetFirewallRule -Name *ssh*
```

### Step 3: Test SSH Access

From your **Mac terminal**:

```bash
# Replace with your Windows VM details
ssh username@windows-vm-ip

# Example
ssh adjamwal@192.168.1.100
```

**First connection**:
- You'll see a fingerprint warning - type `yes`
- Enter your Windows password
- You should see Windows PowerShell prompt

### Step 4: Set Up SSH Key Authentication (Recommended)

**On your Mac**:

```bash
# Generate SSH key if you don't have one
ssh-keygen -t ed25519 -C "your_email@example.com"
# Press Enter for all prompts to use defaults

# Copy public key to Windows VM
# Note: Windows uses a different location for authorized_keys
cat ~/.ssh/id_ed25519.pub
# Copy the output
```

**On Windows VM**:

```powershell
# Create .ssh directory in your user profile
New-Item -Path "$env:USERPROFILE\.ssh" -ItemType Directory -Force

# Create authorized_keys file
# Paste your public key into this file
notepad "$env:USERPROFILE\.ssh\authorized_keys"

# Set proper permissions
icacls "$env:USERPROFILE\.ssh\authorized_keys" /inheritance:r
icacls "$env:USERPROFILE\.ssh\authorized_keys" /grant:r "$env:USERNAME`:R"
```

**Test key-based login from Mac**:

```bash
ssh username@windows-vm-ip
# Should login without password
```

### Step 5: Configure PowerShell as Default Shell (Optional but Recommended)

```powershell
# Set PowerShell as default SSH shell instead of cmd.exe
New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShell -Value "C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe" -PropertyType String -Force

# Or use PowerShell 7 if installed
New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShell -Value "C:\Program Files\PowerShell\7\pwsh.exe" -PropertyType String -Force
```

---

## Part 2: Setup Remote Development in Windsurf/VS Code

### Option A: Using VS Code Remote-SSH Extension

**Step 1: Install Remote-SSH Extension**

1. Open VS Code/Windsurf
2. Go to Extensions (⌘+Shift+X)
3. Search for **"Remote - SSH"**
4. Install **Remote - SSH** by Microsoft
5. Also install **Remote - SSH: Editing Configuration Files**

**Step 2: Configure SSH Connection**

1. Press **⌘+Shift+P** (Command Palette)
2. Type **"Remote-SSH: Connect to Host..."**
3. Select **"Configure SSH Hosts..."**
4. Choose your SSH config file (usually `~/.ssh/config`)
5. Add your Windows VM:

```ssh-config
Host windows-build-vm
    HostName 192.168.1.100
    User adjamwal
    IdentityFile ~/.ssh/id_ed25519
    ForwardAgent yes
```

**Step 3: Connect to Windows VM**

1. Press **⌘+Shift+P**
2. Type **"Remote-SSH: Connect to Host..."**
3. Select **"windows-build-vm"**
4. New window opens - you're now connected!
5. **File** → **Open Folder** → Navigate to `C:\Workspace\WindowsUnifiedConnector`

**Step 4: Install Extensions on Remote**

When connected to Windows VM, install these extensions **on the remote**:
- C/C++ (Microsoft)
- CMake Tools (if using CMake)
- PowerShell
- MSBuild Tools (if available)

### Option B: Using SFTP/Manual Sync

If Remote-SSH doesn't work well, you can use SFTP to sync files:

**Install SFTP Extension**:
1. Extensions → Search **"SFTP"**
2. Install **"SFTP/FTP sync"** by Natizyskunk

**Configure SFTP**:
```json
// .vscode/sftp.json
{
    "name": "Windows Build VM",
    "host": "192.168.1.100",
    "protocol": "sftp",
    "port": 22,
    "username": "adjamwal",
    "privateKeyPath": "~/.ssh/id_ed25519",
    "remotePath": "/C:/Workspace/WindowsUnifiedConnector",
    "uploadOnSave": true,
    "downloadOnOpen": false,
    "ignore": [
        ".git",
        "x64",
        "x86",
        "build",
        "*.log"
    ]
}
```

**Usage**:
- ⌘+Shift+P → **"SFTP: Sync Local → Remote"**
- Files auto-upload on save

---

## Part 3: Build on Remote Windows VM

### Method 1: Integrated Terminal (Remote-SSH)

Once connected via Remote-SSH:

```powershell
# Terminal opens on Windows VM automatically
cd C:\Workspace\WindowsUnifiedConnector

# Traditional build
AmpRepositorySync.exe checkout build.spec
cd UCID; .\UpdateUCID.ps1; cd ..
nuget restore WindowsUnifiedConnector.sln
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m

# Or Conan build
conan install . --build=missing -s build_type=Release -s arch=x86_64
msbuild MockLibraries\MockUCID\MockUCID.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m
```

### Method 2: SSH Commands from Mac

```bash
# Run single command
ssh windows-build-vm "cd C:\Workspace\WindowsUnifiedConnector && msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m"

# Interactive session
ssh windows-build-vm
cd C:\Workspace\WindowsUnifiedConnector
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m
```

### Method 3: Build Script

Create `build-remote.sh` on your Mac:

```bash
#!/bin/bash
# Build on remote Windows VM

SSH_HOST="windows-build-vm"
REMOTE_PATH="C:\\Workspace\\WindowsUnifiedConnector"

echo "🔨 Building on Windows VM..."

ssh $SSH_HOST << 'ENDSSH'
cd C:\Workspace\WindowsUnifiedConnector

Write-Host "🔧 Restoring NuGet packages..."
nuget restore WindowsUnifiedConnector.sln

Write-Host "🏗️ Building solution..."
msbuild WindowsUnifiedConnector.sln `
    /p:Configuration=Release `
    /p:Platform=x64 `
    /maxcpucount `
    /verbosity:minimal

Write-Host "✅ Build complete!"
ENDSSH

echo "✅ Done!"
```

Make it executable:
```bash
chmod +x build-remote.sh
./build-remote.sh
```

---

## Part 4: Sync Code Changes

### Method 1: Git-Based Workflow (Recommended)

**On Mac (Local)**:
```bash
# Make changes locally
vim SharedLibs/libLogger/Logger.cpp

# Commit and push to GitHub
git add .
git commit -m "Update logger"
git push origin master
```

**On Windows VM (Remote)**:
```powershell
# Pull latest changes
cd C:\Workspace\WindowsUnifiedConnector
git pull origin master

# Build
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m
```

**Automate it**:
```bash
# build-and-sync.sh on Mac
#!/bin/bash
git push origin master
ssh windows-build-vm "cd C:\Workspace\WindowsUnifiedConnector && git pull && msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m"
```

### Method 2: Direct File Sync via SCP

```bash
# Copy single file
scp SharedLibs/libLogger/Logger.cpp windows-build-vm:C:/Workspace/WindowsUnifiedConnector/SharedLibs/libLogger/

# Copy entire directory
scp -r SharedLibs/ windows-build-vm:C:/Workspace/WindowsUnifiedConnector/

# Rsync (if available on Windows)
rsync -avz --exclude 'x64' --exclude 'x86' ./ windows-build-vm:/C:/Workspace/WindowsUnifiedConnector/
```

### Method 3: Auto-Sync with SFTP Extension

If using SFTP extension, files auto-upload on save.

---

## Part 5: Advanced Setup

### A. Port Forwarding for Debugging

Forward ports from Windows VM to your Mac:

```bash
# Forward port 3000 (example: if service runs on 3000)
ssh -L 3000:localhost:3000 windows-build-vm

# Now access http://localhost:3000 on Mac → hits Windows VM
```

### B. X11 Forwarding (GUI Apps)

If you need to run Windows GUI apps (like Visual Studio):

**This won't work directly (Windows → Mac X11), but you can use RDP instead:**

```bash
# Open RDP to Windows VM
open rdp://windows-build-vm
```

### C. Remote Debugging

**Setup remote debugging in VS Code**:

1. Install **C/C++** extension on remote
2. Create `.vscode/launch.json` on remote:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug UCService",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/x64/Release/UCService.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "console": "integratedTerminal"
        }
    ]
}
```

3. Set breakpoints
4. Press F5 to debug

### D. Watch for Changes and Auto-Build

**On Windows VM**, create a file watcher:

```powershell
# watch-and-build.ps1
$watcher = New-Object System.IO.FileSystemWatcher
$watcher.Path = "C:\Workspace\WindowsUnifiedConnector"
$watcher.IncludeSubdirectories = $true
$watcher.Filter = "*.cpp"
$watcher.EnableRaisingEvents = $true

$action = {
    Write-Host "File changed, rebuilding..."
    msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m /verbosity:minimal
}

Register-ObjectEvent $watcher "Changed" -Action $action

Write-Host "Watching for file changes..."
while ($true) { Start-Sleep -Seconds 1 }
```

---

## Part 6: Workflow Examples

### Scenario 1: Make Changes and Build

**On Mac**:
```bash
# Edit code in Windsurf
# Changes are either:
# - Directly on remote (if using Remote-SSH)
# - Auto-synced (if using SFTP)
# - Manually pushed via git

# Trigger build
./build-remote.sh
```

### Scenario 2: Full Development Cycle

```bash
# 1. Pull latest
ssh windows-build-vm "cd C:\Workspace\WindowsUnifiedConnector && git pull"

# 2. Edit locally (auto-syncs if using Remote-SSH or SFTP)

# 3. Build remotely
ssh windows-build-vm "cd C:\Workspace\WindowsUnifiedConnector && msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m"

# 4. Run tests
ssh windows-build-vm "cd C:\Workspace\WindowsUnifiedConnector && .\x64\Release\TestUcConfig.exe"

# 5. Commit and push
git add .
git commit -m "Feature X"
git push
```

### Scenario 3: Continuous Development

**Terminal 1 (Mac)**: Edit in Windsurf with Remote-SSH

**Terminal 2 (Mac → Windows SSH)**:
```powershell
# Interactive session on Windows VM
cd C:\Workspace\WindowsUnifiedConnector

# Quick rebuild command
function rb { msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m /verbosity:minimal }

# Use it
rb  # Quick rebuild
```

---

## Part 7: Troubleshooting

### Issue 1: SSH Connection Refused

**Symptoms**:
```
ssh: connect to host 192.168.1.100 port 22: Connection refused
```

**Solutions**:
```powershell
# On Windows VM - Check SSH service
Get-Service sshd
Start-Service sshd

# Check firewall
Get-NetFirewallRule -Name *ssh*

# Check port 22 is listening
netstat -an | Select-String ":22"
```

### Issue 2: Permission Denied (Public Key)

**Symptoms**:
```
Permission denied (publickey,password,keyboard-interactive).
```

**Solutions**:
```powershell
# On Windows VM - Check authorized_keys
cat $env:USERPROFILE\.ssh\authorized_keys

# Fix permissions
icacls "$env:USERPROFILE\.ssh\authorized_keys" /inheritance:r
icacls "$env:USERPROFILE\.ssh\authorized_keys" /grant:r "$env:USERNAME`:R"

# Restart SSH service
Restart-Service sshd
```

### Issue 3: Path Issues in Remote Terminal

**Symptoms**:
```
msbuild: command not found
```

**Solutions**:
```powershell
# Add to PowerShell profile on Windows VM
notepad $PROFILE

# Add these lines:
$env:PATH += ";C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin"
$env:PATH += ";C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64"

# Or use Developer Command Prompt
& "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
```

### Issue 4: Remote-SSH Extension Not Connecting

**Solutions**:
1. Check SSH config file syntax
2. Try connecting via regular SSH first
3. Check VS Code/Windsurf logs: **View** → **Output** → Select "Remote-SSH"
4. Try installing Remote-SSH: Nightly instead
5. Restart Windsurf/VS Code

### Issue 5: File Sync Issues

**SFTP not uploading**:
- Check `.vscode/sftp.json` config
- ⌘+Shift+P → "SFTP: List All" to debug
- Check SSH key permissions

**Git conflicts**:
```bash
# On Windows VM - Hard reset if needed
git fetch origin
git reset --hard origin/master
```

---

## Part 8: Performance Tips

### 1. Use Persistent SSH Connection

**On Mac**, create `~/.ssh/config`:
```ssh-config
Host windows-build-vm
    HostName 192.168.1.100
    User adjamwal
    IdentityFile ~/.ssh/id_ed25519
    ForwardAgent yes
    
    # Keep connection alive
    ServerAliveInterval 60
    ServerAliveCountMax 3
    
    # Reuse connections (faster)
    ControlMaster auto
    ControlPath ~/.ssh/sockets/%r@%h:%p
    ControlPersist 600
```

Create sockets directory:
```bash
mkdir -p ~/.ssh/sockets
```

### 2. Exclude Build Artifacts from Sync

**If using SFTP**, add to ignore list:
```json
"ignore": [
    ".git",
    "x64",
    "x86",
    "Win32",
    "Debug",
    "Release",
    "build",
    "*.log",
    "*.obj",
    "*.pdb",
    ".vs",
    "packages"
]
```

### 3. Incremental Builds

```powershell
# Only rebuild changed projects (MSBuild does this automatically)
msbuild WindowsUnifiedConnector.sln /p:Configuration=Release /p:Platform=x64 /m

# Clean only when needed
msbuild WindowsUnifiedConnector.sln /t:Clean
```

### 4. Use Build Cache

```powershell
# MSBuild automatically caches
# Conan caches in C:\Users\<user>\.conan2\
```

---

## Quick Reference

### SSH Config Template
```ssh-config
Host windows-build
    HostName 192.168.1.100
    User adjamwal
    IdentityFile ~/.ssh/id_ed25519
    ForwardAgent yes
    ServerAliveInterval 60
```

### Quick Commands

```bash
# Connect
ssh windows-build

# Run command
ssh windows-build "cd C:\Workspace\WindowsUnifiedConnector && msbuild ..."

# Copy files
scp file.cpp windows-build:C:/Workspace/WindowsUnifiedConnector/

# Port forward
ssh -L 3000:localhost:3000 windows-build
```

### VS Code Remote-SSH

```
⌘+Shift+P → "Remote-SSH: Connect to Host"
Select: windows-build
File → Open Folder → C:\Workspace\WindowsUnifiedConnector
Terminal opens → run build commands
```

---

## Recommended Workflow

**My recommended setup**:

1. ✅ **Use Remote-SSH** for direct remote development
2. ✅ **Keep Git repo on both** Mac and Windows VM
3. ✅ **Edit on Windows VM** via Remote-SSH (code executes there)
4. ✅ **Build on Windows VM** via integrated terminal
5. ✅ **Commit from Mac** (after closing remote session)

**Why?**
- Best performance (no file sync delays)
- Full IDE features
- Direct access to build tools
- Easy debugging

---

## Next Steps

1. ☑️ Set up SSH server on Windows VM
2. ☑️ Test SSH connection from Mac
3. ☑️ Install Remote-SSH extension in Windsurf/VS Code
4. ☑️ Connect to Windows VM remotely
5. ☑️ Open WindowsUnifiedConnector folder
6. ☑️ Run build in integrated terminal
7. ☑️ Enjoy seamless development! 🎉

---

**Last Updated**: January 22, 2026  
**Works with**: Windsurf, VS Code, any SSH-capable IDE
