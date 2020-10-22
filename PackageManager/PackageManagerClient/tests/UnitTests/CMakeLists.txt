cmake_minimum_required (VERSION 3.10)

add_executable(UnitTests "pmLibTests/TestPmConfig.cpp" "utilTests/TestPmHttp.cpp" "pmLibTests/TestPmManifest.cpp" "pmLibTests/TestCheckinFormatter.cpp")

target_include_directories(UnitTests 
    PRIVATE ${curl_INCLUDE_DIRS}
    )

target_link_directories(UnitTests PRIVATE ${jsoncpp_LIBRARY} )
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