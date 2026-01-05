cmake_minimum_required(VERSION 3.16)
project(customNodesCppExamplePlugin02)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 1. 顯式設定 AR SDK 的路徑
set(AR_STUDIO_RUNTIME_DIR "/home/auo/Basemark/plugin/linux/ar-player-lib")

message(STATUS "DSDK Include Path: ${AR_STUDIO_RUNTIME_DIR}/include")

# 2. 找 OpenCV
find_package(OpenCV REQUIRED)

# 3. 建立插件
add_library(${CMAKE_PROJECT_NAME} SHARED 
    customNodesCppExamplePlugin02.cpp
)

# 4. 包含路徑（確保包含到 include 層級）
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${AR_STUDIO_RUNTIME_DIR}/include"
    ${OpenCV_INCLUDE_DIRS}
)

# 5. 連結庫
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    ${OpenCV_LIBS}
    "${AR_STUDIO_RUNTIME_DIR}/lib/librocksolidArPlayer.so"
    "${AR_STUDIO_RUNTIME_DIR}/lib/librsDsdkCore.so"
)

# 6. 輸出設定
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    BUILD_RPATH "${AR_STUDIO_RUNTIME_DIR}/lib"
)
