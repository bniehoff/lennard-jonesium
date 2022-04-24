# Configure the Cython targets for C++ bindings


# SeedGenerator module
add_cython_target(_seed_generator CXX)

add_library(_seed_generator MODULE ${_seed_generator})

set_target_properties(_seed_generator PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(_seed_generator
    Eigen3::Eigen
    # fmt::fmt
    api
)

python_extension_module(_seed_generator)


# Simulation module
# add_cython_target(_simulation CXX)

# add_library(_simulation MODULE ${_simulation})

# set_target_properties(_simulation PROPERTIES LINKER_LANGUAGE CXX)

# target_link_libraries(_simulation
#     Eigen3::Eigen
#     # fmt::fmt
#     api
# )

# python_extension_module(_simulation)


# The install directory is relative to the project root (where the first CMakeLists.txt was found)
install(TARGETS _seed_generator LIBRARY DESTINATION src/python/lennardjonesium/simulation)
