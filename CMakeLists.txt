cmake_minimum_required(VERSION 3.22)
project(Cone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_executable(${PROJECT_NAME} src/Main.cpp src/Core/Cone.cpp src/Core/Cone.hpp src/Renderer/Window.cpp src/Renderer/Window.hpp src/Renderer/Context.cpp src/Renderer/Context.hpp src/Renderer/Swapchain.cpp src/Renderer/Swapchain.hpp src/Renderer/Pipeline.cpp src/Renderer/Pipeline.hpp src/Renderer/Framebuffer.cpp src/Renderer/Framebuffer.hpp src/Renderer/Image.cpp src/Renderer/Image.hpp src/Renderer/Renderer.cpp src/Renderer/Renderer.hpp src/Common/Utilities.cpp src/Renderer/Buffer/Buffer.cpp src/Renderer/Buffer/Buffer.hpp src/Renderer/Buffer/VertexBuffer.cpp src/Renderer/Buffer/VertexBuffer.hpp src/Renderer/Buffer/IndexBuffer.cpp src/Renderer/Buffer/IndexBuffer.hpp src/Asset/SubMesh.cpp src/Asset/SubMesh.hpp src/Scene/SceneMember.cpp src/Scene/SceneMember.hpp src/Scene/Scene.cpp src/Scene/Scene.hpp src/Scene/Camera.cpp src/Scene/Camera.hpp src/Asset/Texture.cpp src/Asset/Texture.hpp src/Asset/Material.cpp src/Asset/Material.hpp src/Asset/Mesh.cpp src/Asset/Mesh.hpp src/Asset/AssetManager.cpp src/Asset/AssetManager.hpp)

#target_precompile_headers(${PROJECT_NAME} PRIVATE src/Core/CnPch.hpp)

include_directories(src src/Vendor src/Vendor/glm src/Vendor/VulkanMemoryAllocator/include)

#target_compile_options(Cone PRIVATE
#        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
#        )

# Volk
if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
elseif(APPLE)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_MACOS_MVK)
endif()
add_subdirectory(src/Vendor/volk)
target_link_libraries(${PROJECT_NAME} PRIVATE volk)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(src/Vendor/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# VkBootstrap
add_subdirectory(src/Vendor/vk-bootstrap)
target_link_libraries(${PROJECT_NAME} PRIVATE vk-bootstrap::vk-bootstrap)