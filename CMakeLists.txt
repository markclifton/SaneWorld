cmake_minimum_required (VERSION 3.14)
project (Sandbox)

set(BuildDir ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})

add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/glfw-3.3.4)
add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/glm-0.9.9.8)

file(GLOB SOURCEFILES
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/**.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/glad/src/**.c
)

file(GLOB HEADERFILES
    ${CMAKE_SOURCE_DIR}/*.hpp
)

include_directories(${CMAKE_SOURCE_DIR}/dependencies/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/glfw-3.3.4/include)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/glm-0.9.9.8)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_executable(Sandbox ${SOURCEFILES} ${HEADERFILES})

find_package(OpenGL REQUIRED)

target_link_libraries(Sandbox glm)
target_link_libraries(Sandbox glfw)
target_link_libraries(Sandbox OpenGL::GL)
