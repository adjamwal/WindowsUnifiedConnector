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

## Configuring the UC

### Launch the Demo UI

1. Checkout the UC identity repo `git@code.engine.sourcefire.com:UnifiedConnector/identity.git`
2. Launch browser with CORS disabled `"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" --disable-web-security --user-data-dir="C:/ChromeDevSession"`
3. Open identity/ui/demo.html

### Configure the Demo UI

On the settings page enter the following information:
```
Business ID: hexmen-dev-demo-bus-id
Base URL: Admin: https://admin.qa.uc.amp.cisco.com
Base URL: Package Manager: https://pacman.qa.uc.amp.cisco.com
Max Stats Pages: 100
Page Size: 100
Username: hexmen-dev-demo-bus-id
Password: <qa password>
```

Check `Auto-Load/Refresh Data`. After all the other pages will load data from the business

### UI Usage

* Computers: Lists all registered endpoints
* Installers: Only used when creating a new business
* Package: Used to setup packages for install/upgrade
* Config Files: Use to setup config files. Config files are assoicated with a Package in the Packages page
  * Note. When creating a config file, the path should match the path specified in the catalog ( ie `<FOLDERID_ProgramFiles>/Cisco/Cisco Secure Client/UC/Configuration/uc.json` for the UC config)
* Settings: Connect the UI to a business

### Packages

Packages are defined in the catalog repository. https://code.engine.sourcefire.com/UnifiedConnector/identity-catalog/blob/master/catalog-qa.json. If packages need to be updated, ask Hex-Force QA to make the required changes

### Installing UC
Download and install the latest build from https://clg5-lab-winjenkins.cisco.com/view/UnifiedConnector/job/UnifiedConnector/job/WindowsUnifiedConnector/job/master/. Master currently uses the hexmen-dev-demo-bus-id. Other branches are pointing to a different production (Consummer/Alpha) business

### Making a new Business
If you wish to make a new business follow these steps:
1. Configure the Demo UI as mentioned above
2. Enter a new business ID
3. Copy the business ID into the username field
4. leave the password as the QA password
5. Click on the `Installers` page
6. Click `Generate ID` and `Create Installer`
7. In the UC configuration directory, open bs.json and replace the business id and installer key with the ones generated
8. Restart the UC. It will join the new business

## UC Demos
[Demo Link](https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector/wiki/UC-Demos)
