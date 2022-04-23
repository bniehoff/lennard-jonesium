# Configure the Cython target for C++ bindings

add_cython_target(bindings CXX)

add_library(bindings MODULE ${bindings}
    # Put any (local) .hpp, .cpp sources here
)

set_target_properties(bindings PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(bindings
    # Eigen3::Eigen
    # fmt::fmt
    greeter
)

python_extension_module(bindings)

# The install directory is relative to the project root (where the first CMakeLists.txt was found)
install(TARGETS bindings LIBRARY DESTINATION src/python/lennardjonesium/bindings)
