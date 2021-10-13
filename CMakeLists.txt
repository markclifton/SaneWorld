cmake_minimum_required (VERSION 3.14)
project (Sandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BuildDir ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BuildDir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BuildDir}/libs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BuildDir}/libs)

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/sane)

file(GLOB_RECURSE SRC_FILES
    src/*.c
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/deps/sane
)

target_link_libraries(${PROJECT_NAME} Sane)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/res/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)