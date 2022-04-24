# Configure the Cython target for C++ bindings

add_cython_target(_parameters CXX)

add_library(_parameters MODULE ${_parameters})

set_target_properties(_parameters PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(_parameters
    Eigen3::Eigen
    # fmt::fmt
    api
)

python_extension_module(_parameters)


add_cython_target(_simulation CXX)

add_library(_simulation MODULE ${_simulation})

set_target_properties(_simulation PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(_simulation
    Eigen3::Eigen
    # fmt::fmt
    api
)

python_extension_module(_simulation)


# The install directory is relative to the project root (where the first CMakeLists.txt was found)
install(TARGETS _parameters _simulation LIBRARY DESTINATION src/python/lennardjonesium/simulation)
