# Lennard-Jones particle simulation

19.09.2021

Ben Niehoff

## Overview

We want to write a molecular dynamics simulator for a large (~10k) number of particles interacting
via the Lennard-Jones potential at a fixed total energy. We will use the Velocity-Verlet method
to integrate Newton's equations of motion for the system, and also measure relevant thermodynamic
properties, such as temperature, pressure, heat capacity, and mean square displacement. Our
end-goal is to obtain a phase diagram in the temperature-pressure plane showing the solid-fluid
phase transition of the system. If it does not cause too much loss of efficiency, we would also
like to log the history of the system evolution, so that it can be played back as an animation.

To interact with the simulation library and view the results, we would like to use a Jupyter
Notebook, so we structure the library as a Python package. However, we implement the actual
physics in C++. We use Cython as a glue to bind these together, so that the internal C++ library
can present itself as a Python package.

We also choose this overall structure to satisfy a secondary goal: to learn and practice using
modern features of C++, and to learn how to get C++ and Python to work together.

## Context

A previous implementation of a Lennard-Jones molecular dynamics simulator exists at
<https://github.com/bniehoff/lennard-jones-particles-c>, which is written as a console application
in C, and is invoked via a Python script which runs it multiple times to gather data at different
points in pressure-temperature space. The C code itself is quite poorly written: it uses a single
source code file, a great deal of global variables, single-letter variables names, and very scant
documentation.

Our intent is to provide an updated and improved version, written in clean, modular, and readable
C++ code, wrapped in a Python package which can be easily imported.

## Goals

- Simulate ~10k Lennard-Jones particles in an efficient manner so that meaningful physics can be
extracted in a reasonable amount of time (in particular, running the simulation at many values
of pressure and temperature so that the phase diagram can be mapped out at some decent resolution).

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
tools may be appropriate for this type of simulation. We are specifically interested in learning
how to design a project in C++ and how to integrate this with Python.

It is also not our goal to delve too deeply into Cython, although it is a tool that is needed in
order to bridge the C++/Python gap most effectively, so we will be using it to that extent.

## Tools used

For representing points in 3-dimensional space, as well as velocity and force vectors, we will use
[Eigen 3](http://eigen.tuxfamily.org/index.php?title=Main_Page). We will not have a need for most
of its linear algebra features, however.

## Technical architecture

The main bottleneck in simulating the physics of the Lennard-Jones potential is in calculating the
inter-particle forces. In principle, this operation must be done pairwise between every pair
of particles, and thus the time required to calculate the forces grows as *O(n^2)*, where *n* is
the number of particles. If the particles are distributed uniformly throughout the volume (which
they generally are), then this corresponds to a scaling of *O(V^2)*, for *V* the volume. This
naive approach is quite costly.

However, there is a way out of this situation, at the cost of some slight loss in accuracy. The
Lennard-Jones potential contains a *1/r^6* attractive term and a *1/r^12* repulsive term. Therefore
the influence of the potential at large distances diminishes as *1/r^6*. This strong falloff
makes the force effectively short-ranged, where a given particle only has meaningful influence
on particles within some characteristic distance, *R*. We therefore approximate the Lennard-Jones
potential by cutting it off completely at a distance *R*.

The effect of this is that, in the force calculation, one only needs to calculate the pairwise
forces between a particle and its nearest neighbors (up to a distance *R*). Since *R* is fixed,
as the volume of the simulation grows, the time needed to perform the force calculation need only
scale as *O(V)* rather than *O(V^2)*, provided we can choose an appropriate way of storing our
particles such that it is easy to pick out the ones up to a distance *R* away.

### Data structures

The [Cell List](https://en.wikipedia.org/wiki/Cell_lists) is an appropriate data structure for this
type of short-range force calculation, which has seen extensive use in the literature since the
earliest molecular dynamics simulations. This was also the technique used in the prior C version of
this project.

We will divide the simulation volume into a number of cubical *Cells* of side length *R*, the cutoff
distance of our short-range force. Then, for any particle in a given Cell, we know that all the
particles within a distance *R* must be in that Cell or one of its 26 neighbors (adjacent along any
faces or diagonals). This means that to calculate the force on a given particle, we must only
iterate over the particles in 27 Cells, rather than in the whole volume.

Since a particle has a sphere of influence of radius *R* and we must iterate over all the particles
distributed (presumed uniformly) in a cubical volume of side length *3R*, this means that still,
only around 16% (the ratio of the volume of a sphere of radius *R* to that of a cube of side *3R*)
of the particles iterated over are relevant for the calculation. So, this could in principle be
improved, although there is an additional overhead associated with increasing the "voxel resolution"
by using smaller Cells, and according to the Wiki article on Cell Lists, there is very little to
gain from this increased complication, so we will keep things simple and use Cells of size *R*.

The Wiki article also describes a data structure based on linked lists, which are appropriate for
large numbers of particles. In our prior experience, the density of particles together with the
cutoff distance *R* imply that a typical Cell in our simulation should only have 10 to 20 particles
in it. So it seems that an array, rather than a linked list, might be able to take better
advantage of memory locality. However, let's do some analysis:

A Cell will contain a number of *Particles*, typically on the order of 10-20 of them. A Particle
must keep track of the following information:

- Position
- Velocity
- Force/Acceleration
- Total Displacement (i.e., position without periodic boundary conditions applied)

The force is equal to the acceleration, since we normalize the particle mass to 1. The total
displacement is needed in addition to the position, because the position will have periodic
boundary conditions applied, and we would like to track the total distance travelled in a
hypothetical infinite volume, so that we can compute the mean square displacement.

Each of these pieces of information is a 3-component vector, where each component is a `double`, or
8 bytes. This gives a 24 bytes for the 3-component vector, which Eigen will pad to 32 for
alignment. Four such vectors is then 128 bytes. My CPU is an Intel i7 10700k, with the following
cache sizes:

- L1: 32KB data + 32KB instructions, per core (8 cores)
- L2: 256KB per core
- L3: 16MB (shared)

At the moment I have no plans to parallelize the simulation. A single core L1 cache (32KB) can hold
262144 particles, which is more than enough for the whole simulation. If the whole simulation fits
in L1 cache, then it seems there is little to be gained by using arrays over linked lists. A single
cache line is 64 bytes, which is only half of a Particle, so there is not likely to be much
performance benefit to using an array over a linked list. Nevertheless, the array implementation
is a bit simpler, so I will stick with that.

## Guideline to the code

The source code is divided into two main parts: a C++ library which does the heavy work of
simulating the physics system, and a Python module which facilitates running and visualizing
simulations. The Python module is still in progress, although the most basic functionality is
there.

The C++ library is divided into six sub-libraries, arranged in a hierarchical structure:

1. [API](cpp/lennardjonesium/api): The interface to C++ or Python for running simulations
2. [Control](cpp/lennardjonesium/control): Controls the flow of execution of the simulation
3. [Output](cpp/lennardjonesium/output): Handles output of data to files
4. [Engine](cpp/lennardjonesium/engine): Sets up initial conditions and integrates the equations of motion
5. [Physics](cpp/lennardjonesium/physics): Deals with physics concepts: forces, momenta, etc.
6. [Tools](cpp/lennardjonesium/tools): Collection of low-level utilities

Each library in this structure is built upon the more basic libraries below it; so, for example,
Control may import names from Physics, but not vice versa. This helps keep everything organized.

Here's a brief walkthrough of what is in each sub-library:

### The API library

In this library, we have three classes:

1. `Simulation`
2. `Configuration`
3. `SeedGenerator`

`Simulation` is the main interface to the C++ library. It takes a set of parameters which describe
everything about the simulation to be run, and also provides methods for running the simulation
either asynchronously (`launch()` and `wait()`) or synchronously (`run()`), which is simply a
wrapper.

`Configuration` is a helper class mostly for interfacing with Python. Since the
`Simulation::Parameters` struct includes many C++ types which are hard to describe in Cython, the
`Configuration` class gives a simpler interface in terms of numeric types and strings. It also
provides the factory function `make_simulation()` which creates a `Simulation` object from this
`Configuration` struct.

`SeedGenerator` is just a thin wrapper around some important functions from the `<random>` header,
in order to make them more readily accessible from Python. This allows both C++ and Python to
generate random seeds in a consistent way, which is important for repeatability of simulations.

### The Control library

The Control library has three main definitions:

1. `SimulationController`
2. `SimulationPhase`
3. `CommandQueue`

`SimulationController` is the "brains" of the simulation. It runs a "schedule" which is a sequence
of `SimulationPhase` instances. The `SimulationController` executes the "main loop" of the
simulation, which means that it processes all of the `Command`s in the `CommandQueue`, until the
`CommandQueue` is empty. Some `Command`s instruct the `SimulationController` to get further
`Command`s from the currently-active `SimulationPhase`, which is how the main loop continues. 
Other `Command`s involve operations like fixing the temperature of the simulation when it drifts,
or writing data to log files, or changing the currently-active `SimulationPhase` to the next one
in the schedule.

`SimulationPhase` manages a particular phase of the simulation, which can be either Equilibration
or Observation. A typical simulation begins with an Equilibration phase, where the velocities are
explicitly adjusted until equilibrium is reached at a requested temperature. After this follows an
Observation phase, where the system is allowed to evolve without interference, and we measure a
number of quantities of interest about it. The `SimulationPhase` object is responsible for
directing each of these different stages of behavior by issuing the appropriate `Command`s based
upon data it receives from the `SimulationController`.

`CommandQueue` is simply a `std::queue` of `Command`s, which encapsulate the notion of instructions
to be performed.

### The Output library

The Output library defines the following:

1. `Logger`
2. `LogMessage`
3. `Dispatcher`
4. `Sink`s

`Logger` is responsible for taking `LogMessage`s and routing them to the appropriate destination
file. Once a `LogMessage` is collected, it is put onto a queue, where it is eventually processed
by the `Dispatcher` which sends it to the appropriate `Sink`, based upon its type. There are four
different `Sink`s, which represent four different files being written by the simulation:

1. Events log (text)
2. Thermodynamics log (`.csv`)
3. Observations log (`.csv`)
4. Snapshots log (`.csv`)

The Events log just contains information about what the `SimulationPhase`s are doing and when
transitions happen between them. This is useful as console output in order to track the progress
of the simulation.

The Thermodynamics log contains measurements of *instantaneous* thermodynamic quantities at every
time step. These are the "raw data" of the simulation, and can be used to perform all necessary
statistical mechanics calculations.

The Observations log contains *aggregate* measurements done over *time*. In principle, everything
in the Observations log can be computed via the appropriate statistical measures on time windows
within the Thermodynamics log. So, if one wanted, one could simply keep the Thermodynamics log and
do post-processing on it. However, I thought it was convenient to generate this information as the
simulation is running.

The Snapshots log contains the positions and velocities of every particle in the system, at a given
time step. For now, this file is used only to record the *final* positions and velocities. But
in principle, the structure of the file allows one to include snapshots from more than one time
step (although it would make the file very large if we attempted to include a lot of snapshots).

### The Engine library

*To be continued...*
