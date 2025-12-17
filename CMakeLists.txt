cmake_minimum_required(VERSION 3.28)

# 設定交叉編譯環境
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# 設定 sysroot 路徑（若使用 sysroot）
# set(CMAKE_SYSROOT /path/to/aarch64/sysroot)

# 設定標準與專案
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(customNodesCppExamplePlugin02)

# 指定 rocksolidArPlayer 的 cmake 路徑
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ar-player-lib/lib/cmake")
find_package(rocksolidArPlayer REQUIRED)

# 建立共享函式庫
add_library(${CMAKE_PROJECT_NAME} SHARED customNodesCppExamplePlugin02.cpp)

# 包含自訂路徑
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ar-player-lib/include")

# 設定輸出目錄
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

# 連結 rocksolidArPlayer 的 custom nodes 模組
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE rocksolidArPlayer::rsCustomNodes)

# 確保只搜尋 aarch64 的函式庫與標頭檔
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

