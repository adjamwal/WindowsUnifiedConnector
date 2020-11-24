cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE pmlib_sources ${CMAKE_SOURCE_DIR}/../PackageManagerClient/*.cpp ${CMAKE_SOURCE_DIR}/../PackageManagerClient/*.h)
add_library (pmlib ${pmlib_sources})

target_include_directories(pmlib 
    PRIVATE "include" ${CMAKE_BINARY_DIR} ${jsoncpp_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/../PackageManagerClient"
    INTERFACE "${CMAKE_SOURCE_DIR}/../PackageManagerClient"
    )

target_link_libraries(pmlib utilLib)

# install rules
install(TARGETS pmlib DESTINATION lib)
install(FILES 
    "${CMAKE_SOURCE_DIR}/../PackageManagerClient/IPmPlatformConfiguration.h" 
    "${CMAKE_SOURCE_DIR}/../PackageManagerClient/IPmPlatformComponentManager.h" 
    "${CMAKE_SOURCE_DIR}/../PackageManagerClient/IPmPlatformDependencies.h"
    "${CMAKE_SOURCE_DIR}/../PackageManagerClient/IPackageManager.h" 
    "${CMAKE_SOURCE_DIR}/../PackageManagerClient/PmTypes.h" 
    DESTINATION include)