cmake_minimum_required (VERSION 3.10)

add_library (mockLib "utilMocks/MockFileUtil.h" "utilMocks/MockFileUtil.cpp" "MocksCommon.h") 

target_link_libraries(mockLib gtest gmock utilLib)

target_include_directories(mockLib 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}"
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/utilMocks"
    )