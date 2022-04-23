from libcpp.string cimport string

cdef extern from "<lennardjonesium/greeter/greeter.hpp>":
    cdef cppclass _Greeter "Greeter":
        _Greeter(int) except +
        void greet(string) except +