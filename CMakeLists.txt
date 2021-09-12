cmake_minimum_required(VERSION 3.18)

project(lennardjones VERSION 0.0.1)

include_directories(${PROJECT_SOURCE_DIR})

if(SKBUILD)
    find_package(PythonExtensions REQUIRED)
    find_package(Cython REQUIRED)

    add_subdirectory(lennardjones)
else()
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    
    include(CTest)
    enable_testing()

    find_package(Catch2 REQUIRED)

    add_executable(tests
        tests/greeter/test_greeter.cpp
        lennardjones/greeter/greeter.cpp
    )
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

    include(Catch)
    catch_discover_tests(tests)
endif()
