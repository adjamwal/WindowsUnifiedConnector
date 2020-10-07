cmake_minimum_required (VERSION 3.10)

add_library (pmlib "src/PackageManager.cpp" 
    "include/IPmConfig.h" 
    "include/IPmPlatformConfiguration.h" 
    "include/IPmPlatformComponentManager.h" 
    "include/IPmPlatformDependencies.h" 
    "include/PmConfig.h" 
    "src/PmConfig.cpp" 
    "include/PmTypes.h" "include/IPmCloud.h" "include/PmCloud.h" "src/PmCloud.cpp" "include/IPmManifest.h" "include/PmManifest.h" "src/PmManifest.cpp" "include/IPackageManager.h" "include/PackageManagerContainer.h" "src/PackageManagerContainer.cpp")

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