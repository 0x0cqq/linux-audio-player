cmake_minimum_required(VERSION 3.10)
project(linux_audio_player LANGUAGES CXX)

set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE DEBUG)
# set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Werror")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


include_directories(include)

include_directories("${CMAKE_SYSROOT}/mnt/usb/ffmpeg/include")
link_directories("${CMAKE_SYSROOT}/mnt/usb/ffmpeg/lib")


add_subdirectory(src)

message("CMAKE_CXX_COMPILER = " ${CMAKE_CXX_COMPILER})
message("CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
MESSAGE("CMAKE_CXX_FLAGS_RELEASE = " ${CMAKE_CXX_FLAGS_RELEASE})
MESSAGE("CMAKE_CXX_FLAGS_DEBUG = " ${CMAKE_CXX_FLAGS_DEBUG})