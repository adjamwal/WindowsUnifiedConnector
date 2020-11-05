cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE mock_sources ./**.cpp ./**.h)
add_library ( mockLib ${mock_sources} )

target_link_libraries(mockLib gtest gmock utilLib pmlib) #ssl_LIBRARY

target_include_directories(mockLib 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${gtest_INCLUDE_DIRS}" "${ssl_INCLUDE_DIRS}" "${pmlib_sources}"
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}"
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/utilMocks"
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/pmLibMocks"
    )
