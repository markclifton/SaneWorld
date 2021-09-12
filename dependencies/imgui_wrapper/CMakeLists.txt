cmake_minimum_required (VERSION 3.14)
project (imgui)

set(BuildDir ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui
    ${CMAKE_SOURCE_DIR}/dependencies/imgui_wrapper/imgui/backends
)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} 
    glfw
    OpenGL::GL
)