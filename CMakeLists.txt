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

find_package(Eigen3 3.4 REQUIRED NO_MODULE)
find_package(fmt REQUIRED)
# find_package(Microsoft.GSL REQUIRED)

# TODO: Make these more portable for different compilers
add_compile_options(
    -Wall -Wextra -Wpedantic -Werror
    # -fsanitize=undefined
    -O3                 # Enable optimization level 3
    -march=native       # Enables vectorization for Eigen in GCC and Clang
    -ffast-math         # Disables adherence to error reporting and rounding rules in GCC
                        # (Needed for efficient std::floor() function)
)

# TODO: Possibly set per-target?
add_compile_definitions(
    BOOST_DISABLE_ASSERTS   # Disables range checking for boost::multi_array
)

# # Make sure submodules are present
# function(update_submodules)
#     execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
#         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#         RESULT_VARIABLE UPDATE_SUBMODULES_RESULT
#     )
#     if(NOT UPDATE_SUBMODULES_RESULT EQUAL "0")
#         message(FATAL_ERROR "git submodule update failed!")
#     endif()
# endfunction()

# find_package(Git QUIET)
# if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
#     update_submodules()
# endif()

# # Add Karnage's lightweight utilities <https://github.com/karnkaul/ktl.git>
# add_subdirectory(external/ktl)

# Define libraries out of which the main library will be built

add_library(greeter STATIC
    src/lennardjonesium/greeter/greeter.hpp
    src/lennardjonesium/greeter/greeter.cpp
)

add_library(draft_cpp23 STATIC
    src/lennardjonesium/draft_cpp23/generator.hpp
    src/lennardjonesium/draft_cpp23/examples.hpp
)

# This is needed because library has no .cpp files
set_target_properties(draft_cpp23 PROPERTIES LINKER_LANGUAGE CXX)

add_library(abstract STATIC
    src/lennardjonesium/abstract/overview.hpp
)

set_target_properties(abstract PROPERTIES LINKER_LANGUAGE CXX)

add_library(tools STATIC
    src/lennardjonesium/tools/aligned_generator.hpp
    src/lennardjonesium/tools/math.hpp
    src/lennardjonesium/tools/bounding_box.hpp
    src/lennardjonesium/tools/bounding_box.cpp
    src/lennardjonesium/tools/system_parameters.hpp
    src/lennardjonesium/tools/cell_list_array.hpp
    src/lennardjonesium/tools/cell_list_array.cpp
    src/lennardjonesium/tools/cubic_lattice.hpp
    src/lennardjonesium/tools/cubic_lattice.cpp
    src/lennardjonesium/tools/moving_sample.hpp
)

add_library(physics STATIC
    src/lennardjonesium/physics/system_state.hpp
    src/lennardjonesium/physics/system_state.cpp
    src/lennardjonesium/physics/forces.hpp
    src/lennardjonesium/physics/derived_properties.hpp
    src/lennardjonesium/physics/derived_properties.cpp
    src/lennardjonesium/physics/measurements.hpp
    src/lennardjonesium/physics/transformations.hpp
    src/lennardjonesium/physics/transformations.cpp
    src/lennardjonesium/physics/observation.hpp
    src/lennardjonesium/physics/analyzers.hpp
    src/lennardjonesium/physics/analyzers.cpp
)

add_library(engine STATIC
    src/lennardjonesium/engine/particle_pair_filter.hpp
    src/lennardjonesium/engine/particle_pair_filter.cpp
    src/lennardjonesium/engine/boundary_condition.hpp
    src/lennardjonesium/engine/boundary_condition.cpp
    src/lennardjonesium/engine/force_calculation.hpp
    src/lennardjonesium/engine/force_calculation.cpp
    src/lennardjonesium/engine/integrator.hpp
    src/lennardjonesium/engine/integrator.cpp
    src/lennardjonesium/engine/initial_condition.hpp
    src/lennardjonesium/engine/initial_condition.cpp
)

add_library(control STATIC
    src/lennardjonesium/control/command_queue.hpp
    src/lennardjonesium/control/simulation_phase.hpp
    src/lennardjonesium/control/simulation_phase.cpp
    src/lennardjonesium/control/simulation.hpp
    src/lennardjonesium/control/simulation.cpp
)

# Link the various dependencies
target_link_libraries(abstract
    PRIVATE Eigen3::Eigen
    PRIVATE draft_cpp23
)

target_link_libraries(tools
    PRIVATE Eigen3::Eigen
    PRIVATE draft_cpp23
)

target_link_libraries(physics
    PRIVATE Eigen3::Eigen
    PRIVATE tools
)

target_link_libraries(engine
    PRIVATE Eigen3::Eigen
    PRIVATE tools
    PRIVATE physics
)

target_link_libraries(control
    PRIVATE Eigen3::Eigen
    PRIVATE fmt::fmt
    PRIVATE tools
    PRIVATE physics
    PRIVATE engine
)

if(SKBUILD)
    # If we are building the Python package, add the subdirectory where __init__.py is
    find_package(PythonExtensions REQUIRED)
    find_package(Cython REQUIRED)

    add_subdirectory(src/lennardjonesium)
else()
    # Otherwise, build the C++ unit tests
    include(CTest)
    enable_testing()

    find_package(Catch2 REQUIRED)

    add_library(mock STATIC
        tests/mock/constant_short_range_force.hpp
        tests/mock/constant_short_range_force.cpp
    )

    target_link_libraries(mock
        PRIVATE Eigen3::Eigen
        # PRIVATE fmt::fmt
        PRIVATE tools
        PRIVATE physics
        PRIVATE engine
        PRIVATE control
    )

    add_executable(unit_tests
        tests/other/test_eigen.cpp
        tests/other/test_modulo.cpp
        tests/other/test_constant_short_range_force.cpp

        tests/lennardjonesium/greeter/test_greeter.cpp

        tests/lennardjonesium/tools/test_bounding_box.cpp
        tests/lennardjonesium/tools/test_cell_list_array.cpp
        tests/lennardjonesium/tools/test_cubic_lattice.cpp
        tests/lennardjonesium/tools/test_moving_sample.cpp

        tests/lennardjonesium/physics/test_system_state.cpp
        tests/lennardjonesium/physics/test_measurements.cpp

        tests/lennardjonesium/engine/test_periodic_boundary_condition.cpp
        tests/lennardjonesium/engine/test_particle_pair_filter.cpp
        tests/lennardjonesium/engine/test_short_range_force_calculation.cpp
        tests/lennardjonesium/engine/test_velocity_verlet_integrator.cpp
        tests/lennardjonesium/engine/test_initial_condition.cpp

        tests/lennardjonesium/control/test_equilibration_phase.cpp
        tests/lennardjonesium/control/test_observation_phase.cpp
    )

    target_link_libraries(unit_tests
        PRIVATE Eigen3::Eigen
        PRIVATE Catch2::Catch2WithMain
        # PRIVATE fmt::fmt
        # PRIVATE Microsoft.GSL::GSL
        # PRIVATE ktl::ktl
        PRIVATE greeter
        PRIVATE mock
        PRIVATE tools
        PRIVATE physics
        PRIVATE engine
        PRIVATE control
    )

    add_executable(integration_tests
        tests/integration/test_system_equilibration.cpp
    )

    target_link_libraries(integration_tests
        PRIVATE Eigen3::Eigen
        PRIVATE Catch2::Catch2WithMain
        # PRIVATE fmt::fmt
        # PRIVATE Microsoft.GSL::GSL
        # PRIVATE ktl::ktl
        PRIVATE mock
        PRIVATE tools
        PRIVATE physics
        PRIVATE engine
        PRIVATE control
    )

    include(Catch)
    catch_discover_tests(unit_tests)
    catch_discover_tests(integration_tests)
endif()
