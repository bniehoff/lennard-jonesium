cmake_minimum_required(VERSION 3.18)

project(lennardjones)

if(SKBUILD)
    find_package(PythonExtensions REQUIRED)
    find_package(Cython REQUIRED)
endif()

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(src)
