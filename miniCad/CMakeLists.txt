cmake_minimum_required(VERSION 3.16)

project(miniCad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCASCADE REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(miniCad
    src/main.cpp
    src/canvas.cpp
    src/shapes.cpp
)

target_link_libraries(miniCad PRIVATE ${OpenCASCADE_LIBRARIES})
target_include_directories(miniCad PRIVATE ${OpenCASCADE_INCLUDE_DIR})


target_link_libraries(miniCad PRIVATE Qt5::Widgets)

set_target_properties(miniCad PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)
set_target_properties(miniCad PROPERTIES
        VS_DEBUGGER_ENVIRONMENT
        "PATH=E:/Qt/5.15.2/msvc2019_64/bin;E:/Tools/vcpkg/installed/x64-windows/bin;%PATH%"
)