cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE mock_sources ${CMAKE_SOURCE_DIR}/../Mocks/**.cpp ${CMAKE_SOURCE_DIR}/../Mocks/**.h)
add_library ( mockLib ${mock_sources} )

target_link_libraries(mockLib gtest gmock utilLib pmlib) #ssl_LIBRARY

target_include_directories(mockLib 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${gtest_INCLUDE_DIRS}" "${ssl_INCLUDE_DIRS}" "${pmlib_sources}" "${CMAKE_SOURCE_DIR}/../Mocks" "${CMAKE_SOURCE_DIR}/../Mocks/PmUtilMocks" "${CMAKE_SOURCE_DIR}/../Mocks/PmMocks"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks/PmUtilMocks"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks/PmMocks"
    )
