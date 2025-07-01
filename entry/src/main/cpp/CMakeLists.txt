# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(MyApplication8)
set(CMAKE_CXX_STANDARD 20)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
ADD_DEFINITIONS(-DVK_USE_PLATFORM_OHOS=1)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)


add_library(entry SHARED napi_init.cpp)
target_link_libraries(entry PUBLIC
    libace_napi.z.so
    libace_ndk.z.so
    libnative_window.so
    libvulkan.so
    libhilog_ndk.z.so
)