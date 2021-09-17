cmake_minimum_required (VERSION 3.14)
project (Sandbox)

set(BuildDir ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BuildDir}/${CMAKE_BUILD_TYPE})

add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/sane-engine)

file(GLOB_RECURSE SRC_FILES
    src/*.c
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/dependencies/sane-engine/src
)

target_link_libraries(${PROJECT_NAME} SaneEngine)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/imgui.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
