cmake_minimum_required (VERSION 3.10)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  # Replace install() to do-nothing macro.
  macro(install)
  endmacro()
  add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
  # Restore original install() behavior.
  macro(install)
    _install(${ARGN})
  endmacro()
endif()

include(GoogleTest)

add_subdirectory(mocks)
add_executable(TestPmConfig pmLibTests/TestPmConfig.cpp )
target_link_directories(TestPmConfig PRIVATE ${jsoncpp_LIBRARY} )
target_link_libraries(TestPmConfig gtest gmock gtest_main pmlib jsoncpp mockLib )

gtest_discover_tests(TestPmConfig
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)
set_target_properties(TestPmConfig PROPERTIES FOLDER tests)