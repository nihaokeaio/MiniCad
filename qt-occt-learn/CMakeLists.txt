cmake_minimum_required(VERSION 3.16)

project(QtOcctLearn LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core)

message(STATUS "Found Qt Core: ${Qt5Core_VERSION}")

add_executable(qt_hello main.cpp)
target_compile_features(qt_hello PRIVATE cxx_std_17)
target_link_libraries(qt_hello PRIVATE Qt5::Core)

if(MSVC)
    target_compile_options(qt_hello PRIVATE /EHsc)
endif()