cmake_minimum_required(VERSION 3.18)

project(lennardjonesium
    VERSION 0.0.1
    DESCRIPTION "A Python C++ extension package for simulating Lennard-Jones particles"
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    src/lennardjonesium/greeter/greeter.hpp
    src/lennardjonesium/greeter/greeter.cpp
    src/lennardjonesium/tools/aligned_vectors.hpp
    src/lennardjonesium/tools/volume.hpp
    src/lennardjonesium/physics/system_state.hpp
    src/lennardjonesium/physics/system_state.cpp
    src/lennardjonesium/physics/interaction.hpp
    src/lennardjonesium/engine/integrator.hpp
    src/lennardjonesium/engine/integrator.cpp
    src/lennardjonesium/engine/velocity_verlet_integrator.hpp
    src/lennardjonesium/engine/velocity_verlet_integrator.cpp
)

set(TEST_SOURCES
    tests/eigen/test_eigen.cpp
    tests/lennardjonesium/greeter/test_greeter.cpp
    tests/lennardjonesium/physics/test_system_state.cpp
    tests/lennardjonesium/engine/test_velocity_verlet_integrator.cpp
)

find_package(Eigen3 3.4 REQUIRED NO_MODULE)

# This option enables vectorization for Eigen in GCC and Clang
# TODO: Make this more portable for different compilers
add_compile_options(-march=native)

if(SKBUILD)
    find_package(PythonExtensions REQUIRED)
    find_package(Cython REQUIRED)

    add_subdirectory(src/lennardjonesium)
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
