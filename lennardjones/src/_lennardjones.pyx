# Cython greeter example
cpdef void hello_cython(str strArg):
    "Prints back 'Hello <param>', for example example: hello.hello('you')"
    print("Hello, {}, and welcome to Cython!".format(strArg))

# C++ greeter example
from libcpp.string cimport string

cdef extern from "<src/greeter/greeter.hpp>":
    string greet(string s)

cpdef void hello_cpp(str s):
    greeting = greet(bytes(s, 'utf-8'))
    print(str(greeting, 'utf-8'))
