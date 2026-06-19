cmake_minimum_required(VERSION 3.10)
project(Lab6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(lab6
    src/main.cpp
    src/CameraProvider.cpp
    src/Display.cpp
    src/FrameProcessor.cpp
    src/KeyProcessor.cpp
    src/MouseHandler.cpp
)

target_link_libraries(lab6 ${OpenCV_LIBS})

add_custom_command(TARGET lab6 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:lab6>/assets
)
