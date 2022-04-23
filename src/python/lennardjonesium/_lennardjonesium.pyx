"""
_lennardjonesium.pyx

Copyright (c) 2021-2022 Benjamin E. Niehoff

This file is part of Lennard-Jonesium.

Lennard-Jonesium is free software: you can redistribute
it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

Lennard-Jonesium is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with Lennard-Jonesium.  If not, see
<https://www.gnu.org/licenses/>.
"""

from lennardjonesium._lennardjonesium cimport _Greeter

def hello_python(name):
    print("Hello, {}, this is a boring function from Python".format(name))

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
