# Cython greeter example
cpdef void hello_cython(str strArg):
    "Prints back 'Hello <param>', for example example: hello_cython('you')"
    print("Hello, {}, and welcome to Cython!".format(strArg))

# C++ greeter example
from libcpp.string cimport string

cdef extern from "<lennardjones/greeter/greeter.hpp>":
    string greet(string s)
    int c_factorial "factorial" (int n)

cpdef void hello_cpp(str s):
    greeting = greet(bytes(s, 'utf-8'))
    print(str(greeting, 'utf-8'))

cpdef int factorial(int n):
    return c_factorial(n)
