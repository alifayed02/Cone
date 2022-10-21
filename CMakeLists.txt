cmake_minimum_required(VERSION 3.22)
project(Cone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_executable(${PROJECT_NAME} src/Main.cpp src/Core/Cone.cpp src/Renderer/Window.cpp src/Renderer/Context.cpp src/Renderer/Swapchain.cpp src/Renderer/Pipeline.cpp src/Renderer/Framebuffer.cpp src/Renderer/Image.cpp)

target_precompile_headers(${PROJECT_NAME} PRIVATE src/Core/CnPch.hpp)

include_directories(src src/Vendor src/Vendor/glm)

# Vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(src/Vendor/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# VkBootstrap
add_subdirectory(src/Vendor/vk-bootstrap)
target_link_libraries(${PROJECT_NAME} vk-bootstrap::vk-bootstrap)

# GLM
add_subdirectory(src/Vendor/glm)
