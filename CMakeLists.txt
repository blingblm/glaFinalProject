cmake_minimum_required(VERSION 3.15)
project(yaku-cpp LANGUAGES CXX)
set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain")
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg triplet")

set(PCRE2_ROOT "D:/vcpkg/installed/x64-windows")
find_path(PCRE2_INCLUDE_DIRS pcre2.h PATHS "${PCRE2_ROOT}/include" REQUIRED)
find_library(PCRE2_8BIT_LIBRARY pcre2-8 PATHS "${PCRE2_ROOT}/lib" REQUIRED)
find_library(PCRE2_POSIX_LIBRARY pcre2-posix PATHS "${PCRE2_ROOT}/lib" REQUIRED)

# message(STATUS "PCRE2 include dir: ${PCRE2_INCLUDE_DIRS}")
# message(STATUS "PCRE2 libraries: ${PCRE2_8BIT_LIBRARY} ${PCRE2_POSIX_LIBRARY}")

# 可执行文件
add_executable(yaku-cpp 
    src/main.cpp
    src/tokenizer.cpp
)
target_include_directories(yaku-cpp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${PCRE2_INCLUDE_DIRS}"
)
target_link_libraries(yaku-cpp PRIVATE
    "${PCRE2_8BIT_LIBRARY}"
    "${PCRE2_POSIX_LIBRARY}"
)
target_compile_definitions(yaku-cpp PRIVATE
    PCRE2_CODE_UNIT_WIDTH=8
)