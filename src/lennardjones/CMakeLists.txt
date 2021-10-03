
# if(SKBUILD)
add_cython_target(_lennardjones CXX)
add_library(_lennardjones MODULE ${_lennardjones}
    greeter/greeter.cpp
)
target_link_libraries(_lennardjones
    PRIVATE Eigen3::Eigen
)
python_extension_module(_lennardjones)

# The install directory is relative to the project root (where the first CMakeLists.txt was found)
install(TARGETS _lennardjones LIBRARY DESTINATION src/lennardjones)
# endif()
