# Windows Unified Connector

## Checkout
```
git clone https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector.git
```

## Bootstrap the client
The UC Service requires a bootstrap file ( bs.json ) in order to start. This file is not packaged with the installer. There are two ways to provide this file

1. Command line argument to installer

        msiexec /i Cisco-UC-Installer-x64.msi BS_CONFIG=<bs.json>

2. Drop the file in `C:\Program Files\Cisco\SecureXYZ\Unified Connector\Configuration\bs.json`. Then start the service

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

## Package UC into AMP

The UC is added as a resource to the common-windows-build. AMP will pick up the UC from common. To update the build packages into common:

1. Run this job https://clg5-lab-winjenkins.cisco.com/view/common/job/FireAMP-Windows-Common-Build-vSphere-vs2015/. This will collect build deployed to nexus and add it to the common pakcage
2. Update the build.spec in the connector repo to use the new common build
3. Start a new connector build. Check the UC option to include UC