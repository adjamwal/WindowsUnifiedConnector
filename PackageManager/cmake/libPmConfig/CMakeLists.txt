cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE libPmConfig_sources ${CMAKE_SOURCE_DIR}/../libPmConfig/*.cpp ${CMAKE_SOURCE_DIR}/../libPmConfig/*.h)
add_library (libPmConfig ${libPmConfig_sources})

target_include_directories(libPmConfig 
    PRIVATE "include" ${CMAKE_BINARY_DIR} ${jsoncpp_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/../libPmConfig"
    INTERFACE "${CMAKE_SOURCE_DIR}/../libPmConfig"
    )

target_link_libraries(libPmConfig libPmUtil)
