"""
linspace.py

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


def linspace(start: float, stop: float, points: int, endpoint: bool = True) -> list[int]:
    """
    Creates a list of `points` evenly-spaced points in the interval [start, stop] (if endpoint
    is True), or [start, stop) (if endpoint is False).

    This is the same as Numpy's linspace() function, but this function uses Python's native float
    type and avoids having a Numpy dependency in the project just to use one function (and not to
    use any of Numpy's most useful features).
    """
    length = stop - start
    denominator = (points - 1) if endpoint else points
    return [start + (count / denominator) * length for count in range(points)]
