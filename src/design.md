# Lennard-Jones particle simulation

19.09.2021

Ben Niehoff

## Overview

We want to write a simulator for a large (~10k) number of particles interacting via the
Lennard-Jones potential at a fixed total energy.  We will use the Velocity-Verlet method to
integrate Newton's equations of motion for the system, and also measure relevant thermodynamic
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
