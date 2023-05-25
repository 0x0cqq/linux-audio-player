cmake_minimum_required(VERSION 3.18)
project(linux_audio_player LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Werror")

include_directories(include)
# should be changed according to the machine configurations
# especially migrate this code to embedded environments
set(INCLUDE_BASE_PATH /usr/include/x86_64-linux-gnu/)
include_directories(${LIB_BASE_PATH}libavcodec)
include_directories(${LIB_BASE_PATH}libavformat)
include_directories(${LIB_BASE_PATH}libavfilter)

set(LIB_BASE_PATH /usr/lib/x86_64-linux-gnu/)

include_directories(${LIB_BASE_PATH}libavcodec)
include_directories(${LIB_BASE_PATH}libavformat)
include_directories(${LIB_BASE_PATH}libavfilter)

add_subdirectory(src)

message("CMAKE_CXX_COMPILER = " ${CMAKE_CXX_COMPILER})
message("CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
