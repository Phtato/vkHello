# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(MyApplication8)
set(CMAKE_CXX_STANDARD 20)

#[[set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_VERT "${SHADERS_DIR}/shader.vert")
set(SHADER_FRAG "${SHADERS_DIR}/shader.frag")

set(RAW_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile")
set(GENERATED_VERT "${RAW_RESOURCES_DIR}/vert.spv")
set(GENERATED_FRAG "${RAW_RESOURCES_DIR}/frag.spv")

file(WRITE "${RAW_RESOURCES_DIR}/check_files.cmake" [[
message(STATUS "Checking required files ...")
foreach(file IN ITEMS "@INPUT_FILE1@" "@INPUT_FILE2@")
    if(NOT EXISTS "${file}")
        message(FATAL_ERROR "Missing required file:${file}")
    endif()
    message(" Found: ${file}")
eachforeach()
]]#[[)

add_custom_command(
    OUTPUT ${GENERATED_VERT} ${GENERATED_FRAG}
    DEPENDS ${SHADER_VERT} ${SHADER_FRAG}
    COMMAND ${CMAKE_COMMAND} -P "${RAW_RESOURCES_DIR}/check_files.cmake"
    COMMAND echo "Running: glslc -fshader-stage=vert ${SHADERS_DIR}/shader.vert -o ${RAW_RESOURCES_DIR}/vert.spv"
    COMMAND glslc -fshader-stage=vert shaders/shader.vert -o ${RAW_RESOURCES_DIR}/vert.spv
    COMMAND echo "Running:glslc -fshader-stage=frag ${SHADERS_DIR}/shader.frag -o ../resources/rawfile/frag.spv"
    COMMAND glslc -fshader-stage=frag shaders/shader.frag -o ../resources/rawfile/frag.spv
    COMMAND ${CMAKE_COMMAND} -E compare_files ${RAW_RESOURCES_DIR} ${GENERATED_VERT}
    COMMAND ${CMAKE_COMMAND} -E compare_files ${RAW_RESOURCES_DIR} ${GENERATED_FRAG}
    COMMENT "Executing pre-build tasks..."
    VERBATIM
)

add_custom_target(PreBuildTasks ALL
  DEPENDS ${GENERATED_VERT} ${GENERATED_FRAG}
)

file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/check_files.cmake"
  CONTENT "$<FILE_CONTENT:${CMAKE_CURRENT_BINARY_DIR}/check_files.cmake>"
  @ONLY
)]]

add_custom_command(
    COMMENT "Compiling shaders..."
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile/vert.spv ${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile/frag.spv
    COMMAND glslc -fshader-stage=vert ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile/vert.spv
    COMMAND glslc -fshader-stage=frag ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile/frag.spv
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag
)

MESSAGE(STATUS "Compiling shaders...")

add_custom_target(CompileShaders ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile/vert.spv ${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile/frag.spv
)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(GLM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include/glm)
ADD_DEFINITIONS(-DVK_USE_PLATFORM_OHOS=1)
find_library(rawfile-lib rawfile.z REQUIRED)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include
                    ${GLM_PATH}
                    "/Users/bilibili/Desktop/stb_image/stb"
                    "~/Library/tinyobjloader")

add_library(entry SHARED napi_init.cpp)
target_link_libraries(entry PUBLIC
    libace_napi.z.so
    libace_ndk.z.so
    libnative_window.so
    libvulkan.so
    libhilog_ndk.z.so
    rawfile.z
)