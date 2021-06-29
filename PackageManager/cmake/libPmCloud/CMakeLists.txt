cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE cloudEvent_sources ${CMAKE_SOURCE_DIR}/../libPmCloud/*.cpp ${CMAKE_SOURCE_DIR}/../libPmCloud/*.h)
add_library (libPmCloud ${cloudEvent_sources})

target_include_directories(libPmCloud 
    PRIVATE "include" ${CMAKE_BINARY_DIR} ${jsoncpp_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/../PmInterface" "${CMAKE_SOURCE_DIR}/../libPmCloud"
    INTERFACE "${CMAKE_SOURCE_DIR}/../libPmCloud"
    )

target_link_libraries(libPmCloud libPmConfig libPmUtil)
