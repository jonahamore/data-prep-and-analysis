cmake_minimum_required(VERSION 3.10)
project(Lab7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(lab7
    src/main.cpp
    src/CameraProvider.cpp
    src/Display.cpp
    src/FrameProcessor.cpp
    src/FaceDetector.cpp
    src/KeyProcessor.cpp
    src/MouseHandler.cpp
)

target_link_libraries(lab7 ${OpenCV_LIBS} Threads::Threads)

add_custom_command(TARGET lab7 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:lab7>/assets
)
