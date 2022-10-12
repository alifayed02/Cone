cmake_minimum_required(VERSION 3.22)
project(Cone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_executable(Cone src/Main.cpp src/Core/Cone.cpp src/Core/Cone.hpp src/Common/Utilities.hpp src/Renderer/Window.cpp src/Renderer/Window.hpp src/Renderer/Context.cpp src/Renderer/Context.hpp src/Renderer/Swapchain.cpp src/Renderer/Swapchain.hpp)

target_precompile_headers(Cone PUBLIC src/Core/CnPch.hpp)

include_directories(src src/Vendor)
add_subdirectory(src/Vendor/vk-bootstrap)

target_link_libraries(Cone vk-bootstrap::vk-bootstrap)

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLM REQUIRED glm)
include_directories(${GLM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GLM_LIBRARY_DIRS})

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)