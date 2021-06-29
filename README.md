# Windows Unified Connector

## Checkout
```
git clone https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector.git
```

## Bootstrap the client
The UC Service requires a bootstrap file ( bs.json ) in order to start. This file is not packaged with the installer. There are two ways to provide this file

1. Command line argument to installer

        msiexec /i Cisco-UC-Installer-x64.msi BS_CONFIG=<bs.json>

2. Drop the file in `C:\Program Files\Cisco\SecureClient|Immunet\Unified Connector\Configuration\bs.json`. Then start the service

An example file is provided at https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector/blob/master/Resources/config/bs.json

## Wix Toolkit
The solution is using the WIX Toolkit to build the installers
The toolkit can be installed from the link below. Visual studio extensions are also provided so VS can use the Wix toolchain to build the installer right from VS. Its actually pretty seamless. Minus grievances listed below 

### Links
Toolkit download https://wixtoolset.org/releases/

Reference manual and tutorials: https://wixtoolset.org/documentation/

## Deploying to Nexus (subject to change)

Dev builds are manually published to `raw/UnifiedConnector/Windows/Pub/x64` in nexus.
To update these builds, in nexus, update uc-0.0.1.alpha.msi and uc-0.0.1.alpha.msi.txt using the upload button and provide the new installer and build number file

* uc-0.0.1.alpha.msi = file from Jenkins master

* uc-0.0.1.alpha.msi.txt - populate this file with the actual build version.

Repeat for uc-99.0.0.alpha.msi/uc-99.0.0.alpha.msi.txt

## Branch Strategy 

Current Branches are:

* master: Main development branch. PRs for UC Beta milestone should be raised here
* enterprise: Alpha Release Candidate branch. PRs with Alpha bug fixes and configuration updates should be rasie here
* consumer: Stale consumer branch. Will be updated when we decide to raise Immunet UC from the dead

### Merge Strategy 
* Use `Squash and Merge` when merging a development PR from a developer fork to any of the branches above
* Use `Merge Pull Request` when syncing any of the release branches above. ( ie. Merging master to enterprise )