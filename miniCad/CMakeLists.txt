cmake_minimum_required(VERSION 3.16)

project(miniCad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCASCADE REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        Data/*.cpp
        Utils/*.cpp
        Rendering/*.cpp
        Presentation/*.cpp
        UI/*.cpp
        *.h
        AppContext.h
        AppContext.cpp
        main.cpp
)

add_executable(
        miniCad
        ${SRC_FILES}
)

target_link_libraries(miniCad PRIVATE ${OpenCASCADE_LIBRARIES})
target_include_directories(miniCad PRIVATE
        ${OpenCASCADE_INCLUDE_DIR}
        .
        Rendering
        Data
        Utils
        Presentation
)


target_link_libraries(miniCad PRIVATE
        Qt5::Widgets
        Qt5::Core
)

set_target_properties(miniCad PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)
set_target_properties(miniCad PROPERTIES
        VS_DEBUGGER_ENVIRONMENT
        "PATH=E:/Qt/5.15.2/msvc2019_64/bin;E:/Tools/vcpkg/installed/x64-windows/bin;%PATH%"
)