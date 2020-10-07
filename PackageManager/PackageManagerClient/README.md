# Package Manager Client

The package manger client is a library that provides access to the package manager cloud service. This is a cross platform library that provides the core package maanger functionality. It's purpose is to:
* Discover software and it's configurations on a local client
* Provide this information to the PM cloud
* Retrieve a list of packages/configrations to update
* orchestrate the updates on the client

## Building the client

### Cmake

The client can be built using cmake using the following commands
```
mkdir build
cmake ../ . \
  -Djsoncpp_INCLUDE_DIRS=<jsoncpp_include_dir> -Djsoncpp_LIBRARY=<jsoncpp_lib_dir> \
  -Dcurl_INCLUDE_DIRS=<curl_include_dir> -Dcurl_LIBRARY=<curl_lib_dir> \
  -Dciscossl_INCLUDE_DIRS=<ciscossl_include_dir> -Dciscossl_LIBRARY=<ciscossl_lib_dir>
cmake --build
cmake --install
```

### Windows

On windows the client can be build using the cmake instructions about. Additionally, a CMakeSettings.json file has been provided. This allows the project to be opened in Visual Studio 2019. An environment variable for `IMN_COMMON` will have to be provided. This should point to and instance of the cloud/common repository. The `windows-common-build` package will also need to be checked out to `IMN_COMMON/common-windows-build`. This package includes the jsoncpp/curl/ciscossl dependencies mentioned about

## Integrating the client
Fill this in later