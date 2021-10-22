cmake_minimum_required(VERSION 3.19)

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
    src/lennardjonesium/physics/operator.hpp
    src/lennardjonesium/physics/operator.cpp
    src/lennardjonesium/engine/interaction.hpp
    src/lennardjonesium/engine/boundary_condition.hpp
    src/lennardjonesium/engine/boundary_condition.cpp
    src/lennardjonesium/engine/integrator.hpp
    src/lennardjonesium/engine/integrator.cpp
    src/lennardjonesium/engine/velocity_verlet_integrator.hpp
    src/lennardjonesium/engine/velocity_verlet_integrator.cpp
)

set(TEST_SOURCES
    tests/eigen/test_eigen.cpp
    tests/lennardjonesium/greeter/test_greeter.cpp
    tests/lennardjonesium/physics/test_system_state.cpp
    tests/lennardjonesium/physics/test_operator.cpp
    tests/lennardjonesium/engine/test_boundary_condition.cpp
    tests/lennardjonesium/engine/test_velocity_verlet_integrator.cpp
)

find_package(Eigen3 3.4 REQUIRED NO_MODULE)

# TODO: Make these more portable for different compilers
add_compile_options(
    -O3                 # Enable optimization level 3
    -march=native       # Enables vectorization for Eigen in GCC and Clang
    -ffast-math         # Disables adherence to error reporting and rounding rules in GCC
                        # (Needed for efficient std::floor() function)
)

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
