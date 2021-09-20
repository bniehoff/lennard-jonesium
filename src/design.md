# Lennard-Jones particle simulation

19.09.2021

Ben Niehoff

## Overview

We want to write a molecular dynamics simulator for a large (~10k) number of particles interacting
via the Lennard-Jones potential at a fixed total energy.  We will use the Velocity-Verlet method
to integrate Newton's equations of motion for the system, and also measure relevant thermodynamic
properties, such as temperature, pressure, heat capacity, and mean square displacement.  Our
end-goal is to obtain a phase diagram in the temperature-pressure plane showing the solid-fluid
phase transition of the system.  If it does not cause too much loss of efficiency, we would also
like to log the history of the system evolution, so that it can be played back as an animation.

To interact with the simulation library and view the results, we would like to use a Jupyter
Notebook, so we structure the library as a Python package.  However, the simulation itself is
speed-critical in a way that is not well-suited to Python, so we implement the actual physics
in C++.  We use Cython as a glue to bind these together, so that the internal C++ library can
present itself as a Python package.

We also choose this overall structure to satisfy a secondary goal: to learn and practice using
modern features of C++, and to learn how to get C++ and Python to work together.

## Context

A previous implementation of a Lennard-Jones molecular dynamics simulator exists at
<https://github.com/bniehoff/lennard-jones-particles-c>, which is written as a console application
in C, and is invoked via a Python script which runs it multiple times to gather data at different
points in pressure-temperature space.  The C code itself is quite poorly written: it uses a single
source code file, a great deal of global variables, single-letter variables names, and very scant
documentation.

Our intent is to provide an updated and improved version, written in clean, modular, and readable
C++ code, wrapped in a Python package which can be easily imported.

## Goals

- Simulate ~10k Lennard-Jones particles in an efficient manner so that meaningful physics can be
extracted in a reasonable amount of time (say, an hour or so for a detailed tour of the
pressure-temperature plane near the solid-fluid phase transition).

- Observe the phase transition in the results and compare it to the actual phase diagram of Argon.

- Log useful data from the simulation, such as time series, that can be used for visualization.

- Learn how to design a medium-scale project using modern features of C++.

- Learn how to bridge the gap between C++ and Python, so that we can combine the speed of C++
with the user-friendliness of Python.

- Learn how to use some development and testing tools, such as [CMake](https://cmake.org/),
[scikit-build](https://scikit-build.readthedocs.io/en/latest/index.html), and
[Catch2](https://github.com/catchorg/Catch2)

## Non-goals

Although the goal is to write a physics simulation which benefits from compiled code and is thus
not well-suited to plain Python, it is *not* our goal to use Numpy, Scipy, or Numba, even if those
tools may be appropriate for this type of simulation.  We are specifically interested in learning
how to design a project in C++ and how to integrate this with Python.

It is also not our goal to delve too deeply into Cython, although it is a tool that is needed in
order to bridge the C++/Python gap most effectively, so we will be using it to that extent.

## Tools used

For representing points in 3-dimensional space, as well as velocity and force vectors, we will use
[Eigen 3](http://eigen.tuxfamily.org/index.php?title=Main_Page).

## Techinical architecture

The main bottleneck in simulating the physics of the Lennard-Jones potential is in calculating the
inter-particle forces.  In principle, this operation must be done pairwise between every pair
of particles, and thus the time required to calculate the forces grows as *O(n^2)*, where *n* is
the number of particles.  If the particles are distributed uniformly throughout the volume (which
they generally are), then this corresponds to a scaling of *O(V^2)*, for *V* the volume.  This
naive approach is quite costly.

However, there is a way out of this situation, at the cost of some slight loss in accuracy.  The
Lennard-Jones potential contains a *1/r^6* attractive term and a *1/r^12* repulsive term.  Therefore
the influence of the potential at large distances diminishes as *1/r^6*.  This strong falloff
makes the force effectively short-ranged, where a given particle only has meaningful influence
on particles within some characteristic distance, *R*.  We therefore approximate the Lennard-Jones
potential by cutting it off completely at a distance *R*.

The effect of this is that, in the force calculation, one only needs to calculate the pairwise
forces between a particle and its nearest neighbors (up to a distance *R*).  Since *R* is fixed,
as the volume of the simulation grows, the time needed to perform the force calculation need only
scale as *O(V)* rather than *O(V^2)*, provided we can choose an appropriate way of storing our
particles such that it is easy to pick out the ones up to a distance *R* away.

### Data structures

There are a variety of data structures useful for *spatial indexing* which are intended to make
it simpler to answer questions like "Find all the objects within a distance *R* from a given point".
However, in our case, we will take advantage of the fact that *R* is known in advance and is fixed
throughout the calculation.

We will divide the simulation volume into a number of cubical *Cells* of side length *R*.  Then,
for any particle in a given Cell, we know that all the particles within a distance *R* must be
in that Cell or one of its 26 neighbors (adjacent along any faces or diagonals).  This means that
to calculate the force on a given particle, we must only iterate over the particles in 27 Cells,
rather than in the whole volume.

Since a particle has a sphere of influence of radius *R* and we must iterate over all the particles
distributed (presumed uniformly) in a cubical volume of side length *3R*, this means that still,
only around 16% (the ratio of the volume of a sphere of radius *R* to that of a cube of side *3R*)
of the particles iterated over are relevant for the calculation.  So, this could definitely be
improved.  However, the number of particles in a Cell is not too many (we know from prior
experience that it will be around 10 to 20), so it is probably not worth subdividing space in a
more sophisticated way unless it can be done cheaply enough to be faster than ~400 extra iterations
per query.


