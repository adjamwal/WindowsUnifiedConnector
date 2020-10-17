cmake_minimum_required (VERSION 3.10)

add_executable(ComponentTests "utilTests/ComponentTestPmHttp.cpp" "utilTests/WindowsCertStore.h" "utilTests/WindowsCertStore.cpp")

target_include_directories(ComponentTests 
    PRIVATE ${curl_INCLUDE_DIRS}
    )

target_link_directories(ComponentTests PRIVATE ${jsoncpp_LIBRARY} ${curl_LIBRARY} )

if(MSVC) 
    target_link_libraries(ComponentTests gtest gmock gtest_main pmlib jsoncpp mockLib dcl_static ws2_32 libcrypto libssl Crypt32 )
    gtest_discover_tests(ComponentTests
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
else()
    target_link_libraries(ComponentTests gtest gmock gtest_main pmlib jsoncpp mockLib libcurl libcrypto libssl )
    gtest_discover_tests(ComponentTests
        WORKING_DIRECTORY ${PROJECT_DIR}
    )
endif()

set_target_properties(ComponentTests PROPERTIES FOLDER tests)