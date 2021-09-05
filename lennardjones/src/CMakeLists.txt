

add_cython_target(_lennardjones CXX)
add_library(_lennardjones MODULE ${_lennardjones})
python_extension_module(_lennardjones)

install(TARGETS _lennardjones LIBRARY DESTINATION lennardjones)
