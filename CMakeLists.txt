cmake_minimum_required (VERSION 3.8)

project (watery VERSION 0.00.1 LANGUAGES CXX)

add_compile_options(/std:c++17)

include_directories("include")
find_package(Boost preprocessor)
add_subdirectory(test)