# Windows Unified Connector

## Checkout
```
git clone https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector.git
```

## Build
Build the solution from Visual Studio

### CI
Jenkins Build Job: https://clg5-lab-winjenkins.cisco.com/view/UnifiedConnector/job/UnifiedConnector/job/WindowsUnifiedConnector/

### Dependencies
* UCID ( Unified Connector IDentity )
  * [Github](https://code.engine.sourcefire.com/UnifiedConnector/EndpointIdentity)
  * [CI](https://clg5-lab-winjenkins.cisco.com/view/CMID/job/Endpoint_Identity_Win/)
  * The UCID version is specified [here](Directory.Build.props)
* CiscoSSL
* Curl
* FakeFunctionFramework
* GoogleTest/GoogleMock
* Jsoncpp
* Zlib

## Wix Toolkit
The solution is using the WIX Toolkit to build the installers
The toolkit can be installed from the link below. Visual studio extensions are also provided so VS can use the Wix toolchain to build the installer right from VS

### Links
Toolkit download https://wixtoolset.org/releases/

Reference manual and tutorials: https://wixtoolset.org/documentation/

## Branch Strategy 

Following the AMP model 
https://code.engine.sourcefire.com/Cloud/fireamp-win-connector/blob/master/docs/process/branches/branches.md

## Configuring the UC

### SecureX / Device Insights
https://securex.test.iroh.site/

Configuration is done via SecureX. 
* You will have to create an account and link it to your SDC Secure Endpoint account. 
* Intructions are provided on the site.
* You may need to ask QA to enable certain feature flags so the Device Insights tab will appear

### Installing Cloud Management

After the account is set up and the device insights tab is visible you will be able to configure and download Cloud Management. From Device Insights you can download a network or a full installer. Both types of installers embed a bs.json file. After installation, the Cloud Management service should be up and running.

### Packages

Packages are defined in the catalog repository. https://code.engine.sourcefire.com/UnifiedConnector/identity-catalog/blob/master/catalog-qa.json. If packages need to be updated, ask Hex-Force QA to make the required changes

## UC Demos
[Demo Link](https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector/wiki/UC-Demos)
