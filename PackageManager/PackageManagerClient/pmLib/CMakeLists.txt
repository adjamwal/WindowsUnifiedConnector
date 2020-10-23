cmake_minimum_required (VERSION 3.10)

file(GLOB_RECURSE pmlib_sources ./**.cpp ./**.h)
add_library (pmlib ${pmlib_sources})

target_include_directories(pmlib 
    PRIVATE "include" ${CMAKE_BINARY_DIR} ${jsoncpp_INCLUDE_DIRS}
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

target_link_libraries(pmlib utilLib)

# install rules
install(TARGETS pmlib DESTINATION lib)
install(FILES 
    "include/IPmPlatformConfiguration.h" 
    "include/IPmPlatformComponentManager.h" 
    "include/IPmPlatformDependencies.h"
    "include/IPackageManager.h" 
    "include/PmTypes.h" 
    DESTINATION include)