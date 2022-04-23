# Configure the Cython target

# add_cython_target(_lennardjonesium CXX
#     # _lennardjonesium.pxd
#     # _lennardjonesium.pyx
# )

# add_library(_lennardjonesium MODULE ${_lennardjonesium}
#     # Once the C++ library is finished, there should be only one file to put here
#     greeter/greeter.hpp
#     greeter/greeter.cpp
# )

# target_link_libraries(_lennardjonesium
#     Eigen3::Eigen
#     fmt::fmt
#     # Microsoft.GSL::GSL
#     # ktl::ktl
# )

# python_extension_module(_lennardjonesium)

# # The install directory is relative to the project root (where the first CMakeLists.txt was found)
# install(TARGETS _lennardjonesium LIBRARY DESTINATION src/lennardjonesium)

add_cython_target(python_bindings CXX)

add_library(python_bindings MODULE ${python_bindings}
    # Put local .hpp, .cpp sources here (from wrapper directory)
    # wrapper.hpp
    # wrapper.cpp
)

target_link_libraries(python_bindings
    Eigen3::Eigen
    fmt::fmt
    # Microsoft.GSL::GSL
    # ktl::ktl
)

python_extension_module(python_bindings)

# The install directory is relative to the project root (where the first CMakeLists.txt was found)
install(TARGETS python_bindings LIBRARY DESTINATION src/lennardjonesium/bindings)
