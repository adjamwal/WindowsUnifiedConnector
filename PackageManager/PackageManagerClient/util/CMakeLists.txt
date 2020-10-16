cmake_minimum_required (VERSION 3.10)

add_library (utilLib "include/IFileUtil.h" 
    "include/FileUtil.h" 
    "src/FileUtil.cpp" 
    "include/IPmLogger.h" 
    "include/PmLogger.h" 
    "src/PmLogger.cpp" "include/IWorkerThread.h" "include/WorkerThread.h" "src/WorkerThread.cpp" "include/IPmHttp.h" "include/PmHttp.h" "src/PmHttp.cpp")

target_include_directories(utilLib 
    PRIVATE "include" ${curl_INCLUDE_DIRS}
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

target_compile_definitions(utilLib PRIVATE CURL_STATICLIB)

# install rules
install(TARGETS utilLib DESTINATION lib)
install(FILES 
    "include/IPmLogger.h" 
    DESTINATION include)