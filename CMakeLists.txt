cmake_minimum_required(VERSION 3.7.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NAME "VulkanSample")
project(${NAME})

option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)

if(AUTO_LOCATE_VULKAN)
    message(STATUS "Attemting to auto locate Vulkan...")
    find_package(Vulkan REQUIRED)

    if(NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
        set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
        STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
    endif()

    if(NOT Vulkan_FOUND)
        message(STATUS "Failed to locate Vulkan, trying again...")
        if(EXISTS "${Vulkan_PATH}")
            message(STATUS "Successfully located Vulkan SDK: ${VULKAN_PATH}")
        else()
            message(FATAL_ERROR "Unable to locate Vulkan SDK. Please set AUTO_LOCATE_VULKAN option to OFF
                           and specify path manually using VULKAN_SDK and VULKAN_VERSION variables below")
            return()
        endif()
    endif()
else()
    message(STATUS "Attempting to locate Vulkan SDK using predefined path...")
    set(VULKAN_SDK "C:/VulkanSDK")
    set(VULKAN_VERSION "1.3.243.0")
    set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")

    if(NOT EXISTS "${VULKAN_PATH}")
        message(FATAL_ERROR "Unable to locate this Vulkan SDK path: ${VULKAN_PATH}")
        return()
    endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories(${VULKAN_PATH}/Include)
    link_directories(${VULKAN_PATH}/Bin)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)

add_executable(${NAME} ${SOURCES} ${HEADERS})

#target_link_libraries(${NAME} PRIVATE Vulkan::Vulkan)

set_property(TARGET ${NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
set_property(TARGET ${NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug)
set_property(TARGET ${NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/build/Release)
set_property(TARGET ${NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/build/MinSizeRel)
set_property(TARGET ${NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/build/RelWithDebInfo)