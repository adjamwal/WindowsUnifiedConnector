cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE pmlib_sources ${CMAKE_SOURCE_DIR}/../libPmClient/*.cpp ${CMAKE_SOURCE_DIR}/../libPmClient/*.h)
add_library (pmlib ${pmlib_sources})

target_include_directories(pmlib 
    PRIVATE "include" ${CMAKE_BINARY_DIR} ${jsoncpp_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/../libPmClient"
    INTERFACE "${CMAKE_SOURCE_DIR}/../libPmClient"
    )

target_link_libraries(pmlib utilLib)

# install rules
install(TARGETS pmlib DESTINATION lib)
install(FILES 
    "${CMAKE_SOURCE_DIR}/../libPmClient/IPmPlatformConfiguration.h" 
    "${CMAKE_SOURCE_DIR}/../libPmClient/IPmPlatformComponentManager.h" 
    "${CMAKE_SOURCE_DIR}/../libPmClient/IPmPlatformDependencies.h"
    "${CMAKE_SOURCE_DIR}/../libPmClient/IPackageManager.h" 
    "${CMAKE_SOURCE_DIR}/../libPmClient/PmTypes.h" 
    DESTINATION include)