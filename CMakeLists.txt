cmake_minimum_required(VERSION 3.18)

project(lennardjones VERSION 0.0.1)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

if(SKBUILD)
    find_package(PythonExtensions REQUIRED)
    find_package(Cython REQUIRED)

    add_subdirectory(src/lennardjones)
else()
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    
    include(CTest)
    enable_testing()

    find_package(Catch2 REQUIRED)

    add_executable(tests
        tests/lennardjones/greeter/test_greeter.cpp
        src/lennardjones/greeter/greeter.cpp
        tests/lennardjones/components/test_particle.cpp
    )

    target_link_libraries(tests
        PRIVATE Eigen3::Eigen
        PRIVATE Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(tests)
endif()
