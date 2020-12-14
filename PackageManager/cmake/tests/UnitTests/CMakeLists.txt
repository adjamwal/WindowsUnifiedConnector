cmake_minimum_required (VERSION 3.17)

file(GLOB_RECURSE test_sources ${CMAKE_SOURCE_DIR}/../Tests/TestPackageManagerClient/*.cpp ${CMAKE_SOURCE_DIR}/../Tests/TestPmUtil/*.cpp)
add_executable(UnitTests ${test_sources})

target_include_directories(UnitTests 
    PRIVATE ${curl_INCLUDE_DIRS} "${gtest_INCLUDE_DIRS}"
    )

target_link_directories(UnitTests PRIVATE ${jsoncpp_LIBRARY} ${gtest_LIBRARY} )
target_link_libraries(UnitTests gtest gmock gtest_main pmlib jsoncpp mockLib )

if(MSVC) 
    gtest_discover_tests(UnitTests
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
else()
    gtest_discover_tests(UnitTests
        WORKING_DIRECTORY ${PROJECT_DIR}
    )
endif()
set_target_properties(UnitTests PROPERTIES FOLDER tests)