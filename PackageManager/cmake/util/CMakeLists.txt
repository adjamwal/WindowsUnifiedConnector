cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE util_sources ${CMAKE_SOURCE_DIR}/../libPmUtil/*.cpp ${CMAKE_SOURCE_DIR}/../libPmUtil/*.h)
add_library (utilLib ${util_sources})

target_include_directories(utilLib 
    PRIVATE "include" ${curl_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/../libPmUtil/"
    INTERFACE "${CMAKE_SOURCE_DIR}/../libPmUtil"
    )

# install rules
install(TARGETS utilLib DESTINATION lib)
install(FILES 
    "${CMAKE_SOURCE_DIR}/../libPmUtil/IPmLogger.h" 
    DESTINATION include)