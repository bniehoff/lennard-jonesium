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
    src/lennardjonesium/abstract/overview.hpp
    
    src/lennardjonesium/greeter/greeter.hpp
    src/lennardjonesium/greeter/greeter.cpp

    src/lennardjonesium/draft_cpp23/generator.hpp
    src/lennardjonesium/draft_cpp23/examples.hpp

    src/lennardjonesium/tools/bounding_box.hpp
    src/lennardjonesium/tools/bounding_box.cpp
    src/lennardjonesium/tools/cell_list_array.hpp
    src/lennardjonesium/tools/cell_list_array.cpp

    src/lennardjonesium/physics/system_state.hpp
    src/lennardjonesium/physics/system_state.cpp
    src/lennardjonesium/physics/forces.hpp
    
    # src/lennardjonesium/physics/pairwise_force.hpp

    # src/lennardjonesium/engine/force_calculation.hpp
    # src/lennardjonesium/engine/cutoff_force_calculation.hpp
    # src/lennardjonesium/engine/boundary_condition.hpp
    # src/lennardjonesium/engine/periodic_boundary_condition.hpp
    # src/lennardjonesium/engine/periodic_boundary_condition.cpp
    # src/lennardjonesium/engine/dynamics.hpp
    # src/lennardjonesium/engine/dynamics.cpp
    # src/lennardjonesium/engine/integrator.hpp
    # src/lennardjonesium/engine/integrator.cpp
    # src/lennardjonesium/engine/velocity_verlet_integrator.hpp
    # src/lennardjonesium/engine/velocity_verlet_integrator.cpp
)

set(TEST_SOURCES
    tests/other/test_eigen.cpp
    tests/other/test_modulo.cpp

    tests/lennardjonesium/greeter/test_greeter.cpp

    tests/lennardjonesium/tools/test_bounding_box.cpp
    tests/lennardjonesium/tools/test_cell_list_array.cpp

    tests/lennardjonesium/physics/test_system_state.cpp

    # tests/lennardjonesium/physics/constant_pairwise_force.hpp
    # tests/lennardjonesium/physics/constant_pairwise_force.cpp
    # tests/lennardjonesium/physics/test_constant_pairwise_force.cpp

    # tests/lennardjonesium/engine/test_dynamics.cpp
    # tests/lennardjonesium/engine/test_velocity_verlet_integrator.cpp
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
