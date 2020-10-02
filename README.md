# Windows Unified Connector

## Installer
The Installer can install/upgrade/uninstall the service.

For upgrades using the installer this command has to be run. You cannot run the installer directly
```
msiexec /i Installer.msi REINSTALL=ALL REINSTALLMODE=vomus
```

## Bootstrapper
Contains the GUI and selects automatically the 32/64 bit installer to run
The bootstrapper could also be used to download the installer

The msiexec command listed above isn't required when upgrading using the bootstrapper

## Wix Toolkit
The solution is using the WIX Toolkit to build the installers
The toolkit can be installed from the link below. Visual studio extensions are also provided so VS can use the Wix toolchain to build the installer right from VS. Its actually pretty seamless. Minus grievances listed below 

### Links
Toolkit download https://wixtoolset.org/releases/

Reference manual and tutorials: https://wixtoolset.org/documentation/
