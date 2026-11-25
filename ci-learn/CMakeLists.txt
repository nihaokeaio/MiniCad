cmake_minimum_required(VERSION 3.16)

project(CILearn LANGUAGES CXX)

add_executable(ci_hello main.cpp)
target_compile_features(ci_hello PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(ci_hello PRIVATE /EHsc)
endif()
