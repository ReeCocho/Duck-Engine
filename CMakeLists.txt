cmake_minimum_required(VERSION 3.7)

# Project
project(Duck)

# Modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# C++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Packages
find_package(SDL2 REQUIRED)
find_package(VULKAN REQUIRED)

# Includes
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${VULKAN_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/dependencies)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(src/shaders)