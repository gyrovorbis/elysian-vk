cmake_minimum_required(VERSION 3.5)

project(VkRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(ELYSIAN_RENDERER_INCLUDES
    api/renderer/elysian_renderer.hpp
    api/renderer/elysian_renderer_vulkan.hpp
    api/renderer/elysian_renderer_device.hpp
    api/renderer/elysian_renderer_shader.hpp
    api/renderer/elysian_renderer_queue.hpp
    api/renderer/elysian_renderer_resource.hpp
    api/renderer/elysian_renderer_pipeline.hpp
    api/renderer/elysian_renderer_instance.hpp
    api/renderer/elysian_renderer_result.hpp
    api/renderer/elysian_renderer_allocator.hpp
    api/renderer/elysian_renderer_debug_messenger.hpp
    api/renderer/elysian_renderer_render_pass.hpp
    api/renderer/elysian_renderer_descriptor.hpp
    api/renderer/elysian_renderer_command.hpp
    api/renderer/elysian_renderer_buffer.hpp
    api/renderer/elysian_renderer_memory.hpp
    api/renderer/elysian_renderer_image.hpp
    api/renderer/elysian_renderer_semaphore.hpp
    api/renderer/elysian_renderer_frame_buffer.hpp
    api/renderer/elysian_renderer_swap_chain.hpp
    api/renderer/elysian_renderer_compute_pipeline.hpp
    api/renderer/elysian_renderer_physical_device.hpp
    api/renderer/elysian_renderer_debug_log.hpp
    api/renderer/elysian_renderer_object.hpp
    api/renderer/elysian_renderer_query.hpp
    api/renderer/elysian_vk_context.hpp
    api/renderer/elysian_renderer_extensions.hpp
    lib/vma/src/VmaUsage.h)

set(ELYSIAN_RENDERER_SOURCES
    source/elysian_renderer.cpp
    source/elysian_renderer_instance.cpp
    source/elysian_renderer_physical_device.cpp
    source/elysian_renderer_debug_log.cpp
    source/elysian_renderer_object.cpp
    source/elysian_renderer_queue.cpp
    source/elysian_renderer_device.cpp
    source/elysian_renderer_command.cpp
    lib/vma/src/VmaUsage.cpp)

find_library(VULKAN_LIB      vulkan)
find_library(MOLTENVK_LIB    MoltenVK)

FIND_PATH(VULKAN_INCLUDE vulkan/vulkan.h)
FIND_PATH(MOLTEN_INCLUDE mvk_vulkan.h)

message("INC VULKAN: ${VULKAN_INCLUDE}")
message("INC MOLTEN: ${MOLTENVK_INCLUDE}")

message("VULKAN: ${VULKAN_LIB}")
message("MOLTEN: ${MOLTENVK_LIB}")


add_library(VkRenderer STATIC
    ${ELYSIAN_RENDERER_SOURCES}
    ${ELYSIAN_RENDERER_INCLUDES})

target_include_directories(VkRenderer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api ${CMAKE_CURRENT_SOURCE_DIR}/lib/vma/src>
        $<INSTALL_INTERFACE:api>
        ${VULKAN_INCLUDE}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/api
)

target_link_libraries(VkRenderer
    PUBLIC
        ${VULKAN_LIB}
        ${MOLTENVK_LIB}
    )
