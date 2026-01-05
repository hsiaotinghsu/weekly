cmake_minimum_required(VERSION 3.16)
project(webcam_ros2 LANGUAGES CXX)

# -------------------------
# C++ 標準
# -------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -------------------------
# 必須指定 AR Studio Runtime
# -------------------------
if(NOT DEFINED AR_STUDIO_RUNTIME_DIR)
    message(FATAL_ERROR "Please specify -DAR_STUDIO_RUNTIME_DIR=/path/to/ar-player-lib")
endif()
message(STATUS "AR_STUDIO_RUNTIME_DIR = ${AR_STUDIO_RUNTIME_DIR}")

# -------------------------
# 找 OpenCV
# -------------------------
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")

# -------------------------
# 可執行檔
# -------------------------
add_executable(rsbg-files-recorder
    rsbg-files-recorder.cpp
)

# -------------------------
# Include directories
# -------------------------
target_include_directories(rsbg-files-recorder
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}                  # 專案自己 header
        ${AR_STUDIO_RUNTIME_DIR}/include             # AR Player + Data SDK header
        ${OpenCV_INCLUDE_DIRS}                        # OpenCV
)

# -------------------------
# Link libraries (完整路徑，依賴順序正確)
# -------------------------
target_link_libraries(rsbg-files-recorder
    PRIVATE
        ${OpenCV_LIBS}
        ${AR_STUDIO_RUNTIME_DIR}/lib/librocksolidArPlayer.so
        ${AR_STUDIO_RUNTIME_DIR}/lib/librsDsdkRecorder.so
        ${AR_STUDIO_RUNTIME_DIR}/lib/librsDsdkCore.so
)

# -------------------------
# RPATH 設定 (執行時可找到 .so)
# -------------------------
set_target_properties(rsbg-files-recorder PROPERTIES
    BUILD_RPATH "${AR_STUDIO_RUNTIME_DIR}/lib"
    INSTALL_RPATH "${AR_STUDIO_RUNTIME_DIR}/lib"
)

