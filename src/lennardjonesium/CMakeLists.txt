# Configure the Cython target

add_cython_target(_lennardjonesium CXX)

add_library(_lennardjonesium MODULE ${_lennardjonesium}
    # Once the C++ library is finished, there should be only one file to put here
    greeter/greeter.hpp
    greeter/greeter.cpp
)

target_link_libraries(_lennardjonesium
    Eigen3::Eigen
    # Microsoft.GSL::GSL
    ktl::ktl
)

python_extension_module(_lennardjonesium)

# The install directory is relative to the project root (where the first CMakeLists.txt was found)
install(TARGETS _lennardjonesium LIBRARY DESTINATION src/lennardjonesium)
