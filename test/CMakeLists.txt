cmake_minimum_required(VERSION 3.10)

project(VkRendererTest VERSION ${ELYSIAN_RENDERER_VERSION} DESCRIPTION "VkRenderer Tests" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTO_RCC ON)
set(CMAKE_AUTOIC ON)

#find_package(Qt5 COMPONENTS Core Test REQUIRED)

add_executable(VkRendererTests
    main.cpp)

target_link_libraries(VkRendererTests
    VkRenderer)

#add_test(NAME VkRendererTests COMMAND VkRendererTests)
