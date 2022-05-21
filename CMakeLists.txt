cmake_minimum_required(VERSION 3.18)

project(lennardjonesium
    VERSION 0.2.0
    DESCRIPTION "A Python C++ extension package for simulating Lennard-Jones particles"
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/cpp
)

set(CMAKE_CXX_STANDARD 20)

find_package(Eigen3 3.4 REQUIRED NO_MODULE)
find_package(fmt REQUIRED)
# find_package(Microsoft.GSL REQUIRED)

# TODO: Make these more portable for different compilers
add_compile_options(
    -Wall -Werror -Wextra -Wpedantic
    # -fsanitize=undefined
    -O3                     # Enable optimization level 3
    -march=native           # Enables vectorization for Eigen in GCC and Clang
    -ffast-math             # Disables adherence to error reporting and rounding rules in GCC
                            # (Needed for efficient std::floor() function)
    -fPIC                   # Position-independent code generation, needed for boost::iostreams
                            # to work in shared library context via Cython
    -fvisibility=hidden     # Make sure only the symbols that are needed are visible
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

add_library(draft_cpp23 STATIC
    src/cpp/lennardjonesium/draft_cpp23/generator.hpp
    src/cpp/lennardjonesium/draft_cpp23/examples.hpp
)

# This is needed because library has no .cpp files
set_target_properties(draft_cpp23 PROPERTIES LINKER_LANGUAGE CXX)

add_library(tools STATIC
    src/cpp/lennardjonesium/tools/aligned_generator.hpp
    src/cpp/lennardjonesium/tools/math.hpp
    src/cpp/lennardjonesium/tools/bounding_box.hpp
    src/cpp/lennardjonesium/tools/bounding_box.cpp
    src/cpp/lennardjonesium/tools/system_parameters.hpp
    src/cpp/lennardjonesium/tools/cell_list_array.hpp
    src/cpp/lennardjonesium/tools/cell_list_array.cpp
    src/cpp/lennardjonesium/tools/cubic_lattice.hpp
    src/cpp/lennardjonesium/tools/cubic_lattice.cpp
    src/cpp/lennardjonesium/tools/moving_sample.hpp
    src/cpp/lennardjonesium/tools/overloaded_visitor.hpp
    src/cpp/lennardjonesium/tools/message_buffer.hpp
    src/cpp/lennardjonesium/tools/text_buffer.hpp
)

add_library(physics STATIC
    src/cpp/lennardjonesium/physics/system_state.hpp
    src/cpp/lennardjonesium/physics/system_state.cpp
    src/cpp/lennardjonesium/physics/forces.hpp
    src/cpp/lennardjonesium/physics/lennard_jones_force.hpp
    src/cpp/lennardjonesium/physics/lennard_jones_force.cpp
    src/cpp/lennardjonesium/physics/derived_properties.hpp
    src/cpp/lennardjonesium/physics/derived_properties.cpp
    src/cpp/lennardjonesium/physics/measurements.hpp
    src/cpp/lennardjonesium/physics/transformations.hpp
    src/cpp/lennardjonesium/physics/transformations.cpp
    src/cpp/lennardjonesium/physics/observation.hpp
    src/cpp/lennardjonesium/physics/analyzers.hpp
    src/cpp/lennardjonesium/physics/analyzers.cpp
)

add_library(engine STATIC
    src/cpp/lennardjonesium/engine/particle_pair_filter.hpp
    src/cpp/lennardjonesium/engine/particle_pair_filter.cpp
    src/cpp/lennardjonesium/engine/boundary_condition.hpp
    src/cpp/lennardjonesium/engine/boundary_condition.cpp
    src/cpp/lennardjonesium/engine/force_calculation.hpp
    src/cpp/lennardjonesium/engine/force_calculation.cpp
    src/cpp/lennardjonesium/engine/integrator.hpp
    src/cpp/lennardjonesium/engine/integrator.cpp
    src/cpp/lennardjonesium/engine/integrator_builder.hpp
    src/cpp/lennardjonesium/engine/initial_condition.hpp
    src/cpp/lennardjonesium/engine/initial_condition.cpp
)

add_library(output STATIC
    src/cpp/lennardjonesium/output/log_message.hpp
    src/cpp/lennardjonesium/output/sinks.hpp
    src/cpp/lennardjonesium/output/sinks.cpp
    src/cpp/lennardjonesium/output/dispatcher.hpp
    src/cpp/lennardjonesium/output/dispatcher.cpp
    src/cpp/lennardjonesium/output/logger.hpp
    src/cpp/lennardjonesium/output/logger.cpp
)

add_library(control STATIC
    src/cpp/lennardjonesium/control/command_queue.hpp
    src/cpp/lennardjonesium/control/simulation_phase.hpp
    src/cpp/lennardjonesium/control/simulation_phase.cpp
    src/cpp/lennardjonesium/control/simulation_controller.hpp
    src/cpp/lennardjonesium/control/simulation_controller.cpp
)

add_library(api STATIC
    src/cpp/lennardjonesium/api/simulation.hpp
    src/cpp/lennardjonesium/api/simulation.cpp
    src/cpp/lennardjonesium/api/seed_generator.hpp
    src/cpp/lennardjonesium/api/configuration.hpp
    src/cpp/lennardjonesium/api/configuration.cpp
    src/cpp/lennardjonesium/api/simulation_buffer.hpp
    src/cpp/lennardjonesium/api/simulation_buffer.cpp
    src/cpp/lennardjonesium/api/simulation_pool.hpp
    src/cpp/lennardjonesium/api/simulation_pool.cpp
)

# Link the various dependencies
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

target_link_libraries(output
    PRIVATE Eigen3::Eigen
    PRIVATE fmt::fmt
    PRIVATE tools
    PRIVATE physics
    PRIVATE engine
)

target_link_libraries(control
    PRIVATE Eigen3::Eigen
    PRIVATE tools
    PRIVATE physics
    PRIVATE engine
    PRIVATE output
)

target_link_libraries(api
    PRIVATE Eigen3::Eigen
    PRIVATE tools
    PRIVATE physics
    PRIVATE engine
    PRIVATE output
    PRIVATE control
)

if(SKBUILD)
    # If we are building the Python package, add the subdirectory where __init__.py is
    find_package(PythonExtensions REQUIRED)
    find_package(Cython REQUIRED)

    add_subdirectory(src/python/lennardjonesium/simulation)
else()
    # Otherwise, build the C++ unit tests
    include(CTest)
    enable_testing()

    find_package(Catch2 REQUIRED)

    add_library(mock STATIC
        tests/cpp/mock/constant_short_range_force.hpp
        tests/cpp/mock/constant_short_range_force.cpp
        tests/cpp/mock/fixed_phases.hpp
        tests/cpp/mock/fixed_phases.cpp
    )

    target_link_libraries(mock
        PRIVATE Eigen3::Eigen
        # PRIVATE fmt::fmt
        PRIVATE tools
        PRIVATE physics
        PRIVATE engine
        PRIVATE output
        PRIVATE control
    )

    add_executable(unit_tests
        tests/cpp/other/test_eigen.cpp
        tests/cpp/other/test_modulo.cpp
        tests/cpp/other/test_reference_unique_ptr.cpp
        tests/cpp/other/test_tee_device.cpp
        tests/cpp/other/test_constant_short_range_force.cpp

        tests/cpp/lennardjonesium/tools/test_bounding_box.cpp
        tests/cpp/lennardjonesium/tools/test_cell_list_array.cpp
        tests/cpp/lennardjonesium/tools/test_cubic_lattice.cpp
        tests/cpp/lennardjonesium/tools/test_moving_sample.cpp
        tests/cpp/lennardjonesium/tools/test_message_buffer.cpp

        tests/cpp/lennardjonesium/physics/test_system_state.cpp
        tests/cpp/lennardjonesium/physics/test_measurements.cpp
        tests/cpp/lennardjonesium/physics/test_lennard_jones_force.cpp

        tests/cpp/lennardjonesium/engine/test_periodic_boundary_condition.cpp
        tests/cpp/lennardjonesium/engine/test_particle_pair_filter.cpp
        tests/cpp/lennardjonesium/engine/test_short_range_force_calculation.cpp
        tests/cpp/lennardjonesium/engine/test_velocity_verlet_integrator.cpp
        tests/cpp/lennardjonesium/engine/test_initial_condition.cpp

        tests/cpp/lennardjonesium/output/test_sinks.cpp
        tests/cpp/lennardjonesium/output/test_dispatcher.cpp
        tests/cpp/lennardjonesium/output/test_logger.cpp

        tests/cpp/lennardjonesium/control/test_equilibration_phase.cpp
        tests/cpp/lennardjonesium/control/test_observation_phase.cpp
        tests/cpp/lennardjonesium/control/test_simulation_controller.cpp

        tests/cpp/lennardjonesium/api/test_simulation.cpp
        tests/cpp/lennardjonesium/api/test_simulation_pool.cpp
    )

    target_link_libraries(unit_tests
        PRIVATE Eigen3::Eigen
        PRIVATE Catch2::Catch2WithMain
        # PRIVATE fmt::fmt
        # PRIVATE ktl::ktl
        PRIVATE mock
        PRIVATE tools
        PRIVATE physics
        PRIVATE engine
        PRIVATE output
        PRIVATE control
        PRIVATE api
    )

    add_executable(integration_tests
        tests/cpp/integration/test_system_equilibration.cpp
        tests/cpp/integration/test_full_simulation.cpp
    )

    target_link_libraries(integration_tests
        PRIVATE Eigen3::Eigen
        PRIVATE Catch2::Catch2WithMain
        # PRIVATE fmt::fmt
        # PRIVATE ktl::ktl
        PRIVATE mock
        PRIVATE tools
        PRIVATE physics
        PRIVATE engine
        PRIVATE output
        PRIVATE control
        PRIVATE api
    )

    include(Catch)

    # Make sure the temp directory exists (otherwise catch_discover_tests fails!)
    set(lj_temp_dir ${PROJECT_SOURCE_DIR}/temp)
    file(MAKE_DIRECTORY ${lj_temp_dir})

    catch_discover_tests(unit_tests WORKING_DIRECTORY ${lj_temp_dir})
    catch_discover_tests(integration_tests WORKING_DIRECTORY ${lj_temp_dir})
endif()
