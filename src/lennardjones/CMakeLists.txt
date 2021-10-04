# Configure the Cython target

add_cython_target(_lennardjones CXX)

add_library(_lennardjones MODULE ${_lennardjones}
    # Once the C++ library is finished, there should be only one file to put here
    greeter/greeter.hpp
    greeter/greeter.cpp
)

target_link_libraries(_lennardjones
    Eigen3::Eigen
)

python_extension_module(_lennardjones)

# The install directory is relative to the project root (where the first CMakeLists.txt was found)
install(TARGETS _lennardjones LIBRARY DESTINATION src/lennardjones)
