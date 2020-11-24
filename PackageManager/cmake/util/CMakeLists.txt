cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE util_sources ${CMAKE_SOURCE_DIR}/../PmUtil/*.cpp ${CMAKE_SOURCE_DIR}/../PmUtil/*.h)
add_library (utilLib ${util_sources})

target_include_directories(utilLib 
    PRIVATE "include" ${curl_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/../PmUtil/"
    INTERFACE "${CMAKE_SOURCE_DIR}/../PmUtil"
    )

# install rules
install(TARGETS utilLib DESTINATION lib)
install(FILES 
    "${CMAKE_SOURCE_DIR}/../PmUtil/IPmLogger.h" 
    DESTINATION include)