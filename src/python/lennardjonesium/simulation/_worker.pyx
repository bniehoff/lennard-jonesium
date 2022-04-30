"""
_worker.pyx

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

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.pair cimport pair

import time

from lennardjonesium.simulation._worker cimport _Worker, _AsyncWorker

cdef class Worker:
    cdef _Worker _worker

    def run(self, count: int = 10):
        self._worker.launch(count)

cdef class AsyncWorker:
    cdef _AsyncWorker _worker

    def launch(self, count: int = 10):
        self._worker.launch(count)
    
    # def get(self):
    #     has_value, value = self._worker.get()
    #     return has_value, str(value, 'utf-8')
    
    def wait(self):
        self._worker.wait()
    
    def eof(self):
        return self._worker.eof()
    
    def read(self):
        return str(self._worker.read(), 'utf-8')
