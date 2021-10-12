cmake_minimum_required(VERSION 3.18)

project(lennardjones
    VERSION 0.0.1
    DESCRIPTION "A Python C++ extension package for simulating Lennard-Jones particles"
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    src/lennardjones/greeter/greeter.hpp
    src/lennardjones/greeter/greeter.cpp
    src/lennardjones/tools/aligned_vectors.hpp
    src/lennardjones/tools/volume.hpp
    src/lennardjones/components/particle.hpp
    src/lennardjones/components/particle.cpp
    src/lennardjones/components/cell.hpp
    src/lennardjones/components/cell.cpp
)

set(TEST_SOURCES
    tests/lennardjones/greeter/test_greeter.cpp
    tests/lennardjones/components/test_particle.cpp
)

find_package(Eigen3 3.4 REQUIRED NO_MODULE)

if(SKBUILD)
    find_package(PythonExtensions REQUIRED)
    find_package(Cython REQUIRED)

    add_subdirectory(src/lennardjones)
else()
    include(CTest)
    enable_testing()

    find_package(Catch2 REQUIRED)

    add_executable(tests
        ${SOURCES}
        ${TEST_SOURCES}
    )

    target_link_libraries(tests
        PRIVATE Eigen3::Eigen
        PRIVATE Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(tests)
endif()
