cmake_minimum_required (VERSION 3.10)

file(GLOB_RECURSE util_sources ./**.cpp ./**.h)
add_library (utilLib ${util_sources})

target_include_directories(utilLib 
    PRIVATE "include" ${curl_INCLUDE_DIRS}
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

# install rules
install(TARGETS utilLib DESTINATION lib)
install(FILES 
    "include/IPmLogger.h" 
    DESTINATION include)