# Configure the Cython targets for C++ bindings


# SeedGenerator module
add_cython_target(_seed_generator CXX)
add_library(_seed_generator MODULE ${_seed_generator})
set_target_properties(_seed_generator PROPERTIES LINKER_LANGUAGE CXX)
# Statically link libstdc++ (Anaconda packages an olf libstc++ which gets in the way otherwise)
target_link_options(_seed_generator
    PUBLIC -static-libstdc++
)
target_link_libraries(_seed_generator
    Eigen3::Eigen
    fmt::fmt
    tools
    physics
    engine
    output
    control
    api
)
python_extension_module(_seed_generator)


# Simulation module
add_cython_target(_simulation CXX)
add_library(_simulation MODULE ${_simulation})
set_target_properties(_simulation PROPERTIES LINKER_LANGUAGE CXX)
# Statically link libstdc++ (Anaconda packages an olf libstc++ which gets in the way otherwise)
target_link_options(_simulation
    PUBLIC -static-libstdc++
)
target_link_libraries(_simulation
    Eigen3::Eigen
    fmt::fmt
    tools
    physics
    engine
    output
    control
    api
)
python_extension_module(_simulation)


# The install directory is relative to the project root (where the first CMakeLists.txt was found)
install(TARGETS _seed_generator _simulation
    LIBRARY DESTINATION src/python/lennardjonesium/simulation)
