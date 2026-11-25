cmake_minimum_required(VERSION 3.16)

project(CILearn LANGUAGES CXX)

add_executable(ci_hello main.cpp)
target_compile_features(ci_hello PRIVATE cxx_std_17)

add_executable(test_add test_add.cpp)
target_compile_features(test_add PRIVATE cxx_std_17)
enable_testing()
add_test(NAME add_basic COMMAND test_add)

if(MSVC)
    target_compile_options(ci_hello PRIVATE /EHsc)
    target_compile_options(test_add PRIVATE /EHsc)
endif()
