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

from lennardjonesium.simulation._worker cimport _Worker

cdef class Worker:
    cdef _Worker _worker

    def run(self):
        self._worker.launch()

        # eof, value = self._worker.poll()

        # while (not eof):
        #     print(str(value, 'utf-8'), flush=True, end="")
            
        #     # We don't want to poll continuously, it will block other threads
        #     time.sleep(0.7)

        #     eof, value = self._worker.poll()
