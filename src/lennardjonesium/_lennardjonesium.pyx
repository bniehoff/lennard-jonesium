from lennardjonesium._lennardjonesium cimport _Greeter

# Cython greeter example
cpdef void hello_cython(str name):
    "Prints back 'Hello <param>', for example example: hello_cython('you')"
    print("Hello, {}, and welcome to Cython!".format(name))

cdef class Greeter:
    cdef _Greeter* _greeter

    def __cinit__(self, number):
        self._greeter = new _Greeter(number)
    
    def __dealloc__(self):
        del self._greeter
    
    def hello(self, str name):
        self._greeter.greet(bytes(name, 'utf-8'))

# cdef extern from "<lennardjonesium/greeter/greeter.hpp>":
#     string greet(string s)
#     int c_factorial "factorial" (int n)

# cpdef void hello_cpp(str s):
#     greeting = greet(bytes(s, 'utf-8'))
#     print(str(greeting, 'utf-8'))

# cpdef int factorial(int n):
#     return c_factorial(n)
