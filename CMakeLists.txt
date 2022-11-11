cmake_minimum_required(VERSION 3.22)
project(Cone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_executable(${PROJECT_NAME} src/Main.cpp src/Core/Cone.cpp src/Core/Cone.hpp src/Renderer/Window.cpp src/Renderer/Window.hpp src/Renderer/Context.cpp src/Renderer/Context.hpp src/Renderer/Swapchain.cpp src/Renderer/Swapchain.hpp src/Renderer/Pipeline.cpp src/Renderer/Pipeline.hpp src/Renderer/Framebuffer.cpp src/Renderer/Framebuffer.hpp src/Renderer/Image.cpp src/Renderer/Image.hpp src/Renderer/Renderer.cpp src/Renderer/Renderer.hpp src/Common/Utilities.cpp src/Renderer/Buffer/Buffer.cpp src/Renderer/Buffer/Buffer.hpp src/Renderer/Buffer/VertexBuffer.cpp src/Renderer/Buffer/VertexBuffer.hpp src/Renderer/Buffer/IndexBuffer.cpp src/Renderer/Buffer/IndexBuffer.hpp src/Scene/Mesh.cpp src/Scene/Mesh.hpp src/Scene/SceneMember.cpp src/Scene/SceneMember.hpp src/Scene/Scene.cpp src/Scene/Scene.hpp src/Scene/Camera.cpp src/Scene/Camera.hpp src/Asset/Texture.cpp src/Asset/Texture.hpp)

#target_precompile_headers(${PROJECT_NAME} PRIVATE src/Core/CnPch.hpp)

include_directories(src src/Vendor src/Vendor/glm src/Vendor/VulkanMemoryAllocator/include src/Vendor/tinygltf)

target_compile_options(Cone PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
        )

# Vulkan
set(Vulkan_LIBRARY $ENV{VULKAN_SDK}/Lib/vulkan-1.lib)
find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
#message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(src/Vendor/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# GLM
add_subdirectory(src/Vendor/glm)

# STB
add_subdirectory(src/Vendor/tinygltf)

# VkBootstrap
add_subdirectory(src/Vendor/vk-bootstrap)
target_link_libraries(${PROJECT_NAME} vk-bootstrap::vk-bootstrap)

# Vulkan Memory Allocator
add_subdirectory(src/Vendor/VulkanMemoryAllocator)