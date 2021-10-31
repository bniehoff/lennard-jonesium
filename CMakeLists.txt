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
    src/lennardjonesium/tools/dimensions.hpp
    src/lennardjonesium/tools/dimensions.cpp
    src/lennardjonesium/tools/cell_list_array.hpp
    src/lennardjonesium/tools/cell_list_array.cpp

    src/lennardjonesium/physics/system_state.hpp
    src/lennardjonesium/physics/system_state.cpp
    src/lennardjonesium/physics/pairwise_force.hpp

    src/lennardjonesium/engine/force_calculation.hpp
    src/lennardjonesium/engine/cutoff_force_calculation.hpp
    src/lennardjonesium/engine/boundary_condition.hpp
    src/lennardjonesium/engine/periodic_boundary_condition.hpp
    src/lennardjonesium/engine/periodic_boundary_condition.cpp
    src/lennardjonesium/engine/dynamics.hpp
    src/lennardjonesium/engine/dynamics.cpp
    # src/lennardjonesium/engine/cell_list_dynamics.hpp
    # src/lennardjonesium/engine/cell_list_dynamics.cpp
    src/lennardjonesium/engine/integrator.hpp
    src/lennardjonesium/engine/integrator.cpp
    src/lennardjonesium/engine/velocity_verlet_integrator.hpp
    src/lennardjonesium/engine/velocity_verlet_integrator.cpp
)

set(TEST_SOURCES
    tests/eigen/test_eigen.cpp

    tests/lennardjonesium/greeter/test_greeter.cpp

    tests/lennardjonesium/tools/test_dimensions.cpp

    tests/lennardjonesium/physics/test_system_state.cpp

    tests/lennardjonesium/engine/test_periodic_boundary_condition.cpp
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

# TODO: Possibly set per-target?
add_compile_definitions(
    BOOST_DISABLE_ASSERTS   # Disables range checking for boost::multi_array
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
