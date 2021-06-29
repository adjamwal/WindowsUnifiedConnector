cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE pmlib_sources ${CMAKE_SOURCE_DIR}/../libPmClient/*.cpp ${CMAKE_SOURCE_DIR}/../libPmClient/*.h)
add_library (libPmClient ${pmlib_sources})

target_include_directories(libPmClient 
    PRIVATE "include" ${CMAKE_BINARY_DIR} ${jsoncpp_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/../PmInterface" "${CMAKE_SOURCE_DIR}/../libPmClient"
    INTERFACE "${CMAKE_SOURCE_DIR}/../libPmClient"
    )

target_link_libraries(libPmClient libPmConfig libPmCloud libPmUtil)

# install rules
install(TARGETS libPmClient DESTINATION lib)
install(FILES 
    "${CMAKE_SOURCE_DIR}/../libPmClient/IPackageManager.h" 
    "${CMAKE_SOURCE_DIR}/../PmInteface/PmTypes.h" 
    "${CMAKE_SOURCE_DIR}/../PmInteface/IPmPlatformConfiguration.h"
    "${CMAKE_SOURCE_DIR}/../PmInteface/IPmPlatformComponentManager.h"
    "${CMAKE_SOURCE_DIR}/../PmInteface/IPmPlatformDependencies.h"

    DESTINATION include)