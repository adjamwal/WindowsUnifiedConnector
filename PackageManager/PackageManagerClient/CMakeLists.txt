cmake_minimum_required(VERSION 3.10)

project (PackageManager VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    if(CMAKE_CL_64)
        set(_PLATFORM_DEFINES "/DWIN64")
    else()
        set(_PLATFORM_DEFINES "/DWIN32")
    endif()
endif()

# jsoncpp
message( "Using jsoncpp jsoncpp_LIBRARY=${jsoncpp_LIBRARY} jsoncpp_INCLUDE_DIRS=${jsoncpp_INCLUDE_DIRS}" )
if(NOT DEFINED jsoncpp_LIBRARY OR NOT DEFINED jsoncpp_INCLUDE_DIRS)
    message( FATAL_ERROR "Missing jsoncpp" )
endif()

# jsoncpp
message( "Using jsoncpp curl_LIBRARY=${curl_LIBRARY} curl_INCLUDE_DIRS=${curl_INCLUDE_DIRS}" )
if(NOT DEFINED curl_LIBRARY OR NOT DEFINED curl_INCLUDE_DIRS)
    message( FATAL_ERROR "Missing curl" )
endif()

# configure a header file to pass the version number only
configure_file(PmVersion.h.in PmVersion.h)

add_subdirectory( util )
add_subdirectory( pmLib )

enable_testing() # Must be in main file

add_subdirectory( tests )