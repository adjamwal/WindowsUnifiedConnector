# Cloud Management Connector
Formerly called Unified Connector

The Cloud Management Connector is an endpoint service that is responsible for managing Cisco security software on the endpoint. It will regularly sync with the Cloud Management Backend (currently called UCB ) and install, update, configure software as required.

## Architecture

 ![](images/UC_Arch.png)

The product has three components

* Cloud Management Service(CMS) - This is the main service that will manage the other components
* Cloud Management Identity(CMID) - This component is responsible for managing the endpoint's identity and session information. This data is provided to other modules
* Cloud Management Package Manager(CMPM) - This component is responsible for checking in with the cloud to determine if any software needs to be installed or updated

## Cloud Management Service
Currently this module is a windows service that is registered during installation. Its role is to ensure the CMID and CMPM modules are running. If a module terminate unexpectedly, the service will restart the module. This module was not built to be cross-platform and currently only supports Windows. 

#### Configuration
The only configuration can be changed in regard to this module is the logging level. The logging level is retrieved from the config file. The logging levels follow a syslog format

```
"uc":{
    "loglevel":7
}
```

#### Bootstrap
When the service starts up, it checks for the presense of a bs.json file. This is the bootstrap file. It contains the information required for CMID to generate an identity. If this file is not present, the service will shutdown.
A sample bs.json is included in the code base https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector/blob/master/Resources/config/bs.json

#### Design
The service itself is very simple. The only tasks this service performs is load the control modules for CMID and CMPM which will spawn the respective processes

## Cloud Management Identity
The CMID module consists of 

* CMID process. This process handles the identity of the endpoint. Various attributes and collected and sent to the cloud to generate an identity as well as session tokens which are required to communicate with the cloud
* CMID control module. This is a shared library that CMS will use to start stop the CMID process. This module also monitors the status of the CMID process and will restart it when necessary 
* CMID API. This API is a shared library that is used to access information from CMID such as business Id, Identity guid, backend URLs and session tokens

More detailed information is provided here https://wiki.cisco.com/display/THEEND/Unified+Connector+-+Identity+Module

#### Configuration
The only configuration can be changed in regard to this module if logging is enabled or disabled. This flag is retrieved from the config file.

```
"id":{
    "enable_verbose_logs":true
}
```

#### Bootstrap
CMID uses the information in the bootstrap file to connect the cloud and generate an identity

## Cloud Management Package Manager
The CMID module consists of 

* CMPM process. This process determines what software is installed and how it's configured on the endpoint. If it determines new software is required it will download an install it automatically
* CMPM control module. This is a shared library that CMS will use to start stop the CMPM process. This module also monitors the status of the CMPM process and will restart it when necessary

### Configuration
The Package Manager allows the following items to be configured via the Cloud

* Allowing Reboot notifications
* The delay between checkins
* The log level. The logging levels follow a syslog format

```
"pm":{
    "AllowPostInstallReboots":true,
    "CheckinInterval":60000,
    "loglevel":7
}
```

In the Package Manager code, you will find there are various other options that will be read from config. These are unofficial options which are levers for QA to change the PM's default behavior to facilitate testing. For example the cache ttl could be shortened from the default value of 1 week so something more reasonable for an end to end test

### OS Interface
The package manager was meant to be a cross platform product. Therefore any OS dependencies have been hidden behind an interface

[PM Interface](https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector/tree/a36fe8d5670a9103a9b9edcfa41ac6b5250f706a/PackageManager/PmInterface)

The interface is currently divided into two main components
* Configuration: This retrieved information about the endpoint including the CM Identity, certificates, application directories and proxy
* ComponentManagement: This provides the functionally to query for installed software as well as installing/updating/removing software on the endpoint

### Workflow
The CMPM has one main worker thread that wakes up periodically to perform its workflow. There are three main sections to the workflow:

* Discovery
* Checkin
* Cleanup

#### Discovery
The discovery portion of the workflow will search the endpoint for any relevant software. The CMPM uses a catalog to determine what to search for. This is an [example catalog](https://code.engine.sourcefire.com/UnifiedConnector/identity-catalog/blob/master/catalog-qa.json). The CMPM will download the catalog and parse the `products` section for a list of items to discover. The CMPM will determine if a product is installed and check if its configuration it up to date. The configuration check is a simple SHA256 of a configuration file. There is no knowledge of what is actually contained in the configuration files.

There are several discovery mechanisms that CMPM can use to find products. Since only the Windows product has been in development so fair the current discovery mechanisms are discovery by:
* MSI product name and publisher
* MSI upgrade code
* Registry keys

The discovery mechanisms are implemented under the PM Interface

#### Checkin
The checkin section will report the list of discovery products/configuration to the cloud. The cloud will respond with a list products and a set of actions to take on each product. The actions the can be returned are:

* Install product
* Update configuration
* Remove product ( Not implemented yet )

The actions for installing/removing products are implement under the PM Interface. Updating the configuration use the std::filesystem library to create/update configuration files.

In the case of install, all installers are downloaded before any of the installers are run. This decision was made because some of the security products will cause network changes which would interrupt any in progress downloads. The installers are downloaded into an installer cache ( temp folder ). The cache is also useful if an installer fails for whatever reason. On the next checkin, CMPM will see the installer is already on disk and will skip the download saving bandwidth

#### Cleanup
The cleanup is the last part of the workflow. The cleanup will attempt to re-send any events that failed to send previously. It will also cleanup any installers that have been left behind in the installer cache

#### Events
Each of the actions will generate an event and report to the cloud if the action was successful or not. There events are defined here https://code.engine.sourcefire.com/UnifiedConnector/identity/blob/master/etc/events-v1.json

#### Proxy
The workflow will kickoff a new thread to handle proxy discovery. Currently CMPM will only discovery system proxies. These include

* WinHttp proxy ( System user only )
* WPAD ( DNS & DHCP )

Authentication is not supported over proxy

### Watchdog
The CMPM has a second thread that acts as a watchdog for the main thread. During the workflow, it will periodically kick the watchdog. If the workflow thread hangs, the watchdog will eventually timeout. When it does it will force a crash and create a dump file so we can examine the hang. We found this was useful in cases where there was a network failure and curl hung and never recovered. We can't plan for all failures so the watchdog is a catch all. After the crash, the CMPM control module will restart the process

### Source code layout
There are currently two main folders for the Package Manager

* [Package Manager](https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector/tree/master/PackageManager)
  * This folder contains the core Package Manager. This folder contains all the cross platform components. This be built with either Visual studios or cmake. (Cmake build may need some love)
  * In the future this folder can live as a submodule but for ease of development it was included in the Windows project
* [Windows Package Manager](https://code.engine.sourcefire.com/UnifiedConnector/WindowsUnifiedConnector/tree/master/WindowsPackageManager)
  * This contains the windows specific parts of the package manager. The main components included here are the code for the windows process and the Windows implementation of the PM Interface

## Crash Handling
All the process us Google breakpad as the crash handler. Cloud Management piggybacks on AMP's infrastructure for crash handling/analysis. When a crash occurs, it will signal AMP's crash handler which will create the dump for the crashing process. The dump will then be upload to AMP's crash server for analysis. If AMP isn't installed, a dump will be created in CM's data directory. The dump will be created by the crashing process so there is no guarantee the dump will be created correctly. This is a temporary measure. At some point CM will have its own crash analysis infrastructure.

## Diagnostics
Cloud Management has a diagnostics tool for support. It will package up the configuration and data directories and create a zip package on the desktop. The tool requires elevated privileges as some data files require elevation to read. And option is provided to run without elevated privileges which will cause those files to be dropped from the package. This was mainly for integration with AnyConnect's DART tool

## References
Older documentation is available on [ampdocs](https://code.engine.sourcefire.com/Cloud/ampdocs/tree/master/connector/win/design/UnifiedConnector)
[Doxygen](https://clg5-lab-winmetrics.cisco.com/winmetrics/dir_90560298f931e8a1469d362f04b8bf68.html)