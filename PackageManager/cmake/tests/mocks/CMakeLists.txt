cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE mock_sources ${CMAKE_SOURCE_DIR}/../Mocks/**.cpp ${CMAKE_SOURCE_DIR}/../Mocks/**.h)
add_library ( mockLib ${mock_sources} )

target_link_libraries(mockLib gtest gmock libPmUtil libPmConfig libPmCloud libPmClient) #ssl_LIBRARY

target_include_directories(mockLib 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${gtest_INCLUDE_DIRS}" "${ssl_INCLUDE_DIRS}" "${pmlib_sources}" "${CMAKE_SOURCE_DIR}/../Mocks" "${CMAKE_SOURCE_DIR}/../Mocks/MockPmUtil" "${CMAKE_SOURCE_DIR}/../Mocks/MockPmClient" "${CMAKE_SOURCE_DIR}/../Mocks/MockPmCloud" "${CMAKE_SOURCE_DIR}/../Mocks/MockPmConfig" "${CMAKE_SOURCE_DIR}/../Mocks/MockPmInterface" "${CMAKE_SOURCE_DIR}/../PmInterface"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks/MockPmUtil"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks/MockPmClient"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks/MockPmCloud"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks/MockPmConfig"
    INTERFACE "${CMAKE_SOURCE_DIR}/../Mocks/MockPmInterface"
    )
