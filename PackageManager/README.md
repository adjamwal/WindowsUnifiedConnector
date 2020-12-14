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
cd cmake
mkdir build
cmake ../ . \
  -Djsoncpp_INCLUDE_DIRS=<jsoncpp_include_dir> -Djsoncpp_LIBRARY=<jsoncpp_lib_dir> \
  -Dcurl_INCLUDE_DIRS=<curl_include_dir> -Dcurl_LIBRARY=<curl_lib_dir> \
  -Dciscossl_INCLUDE_DIRS=<ciscossl_include_dir> -Dciscossl_LIBRARY=<ciscossl_lib_dir> \
  -Dfff_INCLUDE_DIRS=<fff_include_dir> \
  -Dgtest_INCLUDE_DIRS=<gtest_include_dir> -Dgtest_LIBRARY=<gtest_lib_dir>
cmake --build
cmake --install
```

Alternatively if the all dependencies are in the same directories a common path can be provided instead
```
 -Dcommon_INCLUDE_DIRS=<include_dir> -Dcommon_LIBRARY=<library_dir>
```

### Windows

On windows, the pacakge manager projects are integrated into a visual studio solution

## Integrating the client
Fill this in later