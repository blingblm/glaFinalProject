cmake_minimum_required(VERSION 3.15)
project(yaku-cpp)


set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain")
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg triplet")
set(PCRE2_ROOT "D:/vcpkg/installed/x64-windows")
#find package
find_path(PCRE2_INCLUDE_DIRS pcre2.h PATHS "${PCRE2_ROOT}/include" REQUIRED)
find_library(PCRE2_8BIT_LIBRARY pcre2-8 PATHS "${PCRE2_ROOT}/lib" REQUIRED)
find_library(PCRE2_POSIX_LIBRARY pcre2-posix PATHS "${PCRE2_ROOT}/lib" REQUIRED)
#excutabel files
add_executable(yaku src/main.cpp)
#include
target_include_directories(yaku PRIVATE ${PCRE2_INCLUDE_DIRS})
target_link_libraries(yaku PRIVATE  ${PCRE2_8BIT_LIBRARY} ${PCRE2_POSIX_LIBRARY})


