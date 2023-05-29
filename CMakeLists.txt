cmake_minimum_required(VERSION 3.16)
project(linux_audio_player LANGUAGES CXX)

set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE DEBUG)
# set(CMAKE_BUILD_TYPE RELEASE)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Werror")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(include)
# should be changed according to the machine configurations
# especially migrate this code to embedded environments
set(INCLUDE_BASE_PATH /usr/include/x86_64-linux-gnu/)
include_directories(${LIB_BASE_PATH}libavcodec)
include_directories(${LIB_BASE_PATH}libavformat)
include_directories(${LIB_BASE_PATH}libavfilter)
include_directories(${LIB_BASE_PATH}libavutil)

set(LIB_BASE_PATH /usr/lib/x86_64-linux-gnu/)
link_directories(${LIB_BASE_PATH})

add_subdirectory(src)

message("CMAKE_CXX_COMPILER = " ${CMAKE_CXX_COMPILER})
message("CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
MESSAGE("CMAKE_CXX_FLAGS_RELEASE = " ${CMAKE_CXX_FLAGS_RELEASE})
MESSAGE("CMAKE_CXX_FLAGS_DEBUG = " ${CMAKE_CXX_FLAGS_DEBUG})