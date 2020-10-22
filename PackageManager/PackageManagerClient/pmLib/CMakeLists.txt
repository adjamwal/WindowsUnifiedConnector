cmake_minimum_required (VERSION 3.10)

add_library (pmlib "src/PackageManager.cpp" 
    "include/IPmPlatformConfiguration.h" 
    "include/IPmPlatformComponentManager.h" 
    "include/IPmPlatformDependencies.h" 
    "include/PmTypes.h" 

    "include/IPmConfig.h" 
    "include/PmConfig.h" 
    "src/PmConfig.cpp" 

    "include/IPmCloud.h" 
    "include/PmCloud.h" 
    "src/PmCloud.cpp" 

    "include/IPmManifest.h" 
    "include/PmManifest.h" 
    "src/PmManifest.cpp" 

    "include/IPackageManager.h" 
    "include/PackageManagerContainer.h" 
    "src/PackageManagerContainer.cpp"

    "include/IPackageInventoryProvider.h"
    "include/PackageInventoryProvider.h"
    "src/PackageInventoryProvider.cpp"

    "include/ICheckinFormatter.h"
    "include/CheckinFormatter.h"
    "src/CheckinFormatter.cpp"

    "include/ITokenAdapter.h"
    "include/TokenAdapter.h"
    "src/TokenAdapter.cpp"

    "include/ICertsAdapter.h"
    "include/CertsAdapter.h"
    "src/CertsAdapter.cpp"

    "include/ICheckinManifestRetriever.h"
    "include/CheckinManifestRetriever.h"
    "src/CheckinManifestRetriever.cpp"

    "include/IComponentPackageProcessor.h"
    "include/ComponentPackageProcessor.h"
    "src/ComponentPackageProcessor.cpp"

    "include/IManifestProcessor.h"
    "include/ManifestProcessor.h"
    "src/ManifestProcessor.cpp"
    )

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