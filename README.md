# Lennard-Jonesium

Microcanonical simulation of the Lennard-Jones substance and analysis of phase transitions.Completely rebuilt in Python and modern C++ using object-oriented and functional programming principles.

The project is finally "done"!  The C++ part (which does all the hard work) is working well, and the Python wrapper package finally has a usable interface.  With the Python wrapper, one can easily `run()` a single simulation, or `run_sweep()` a whole "sweep" of simulations over parameter space. When using `run_sweep()`, everything about the directory structure of the collection of simulations is managed automatically.

## Design notes

The codebase is a bit large to just dive into (there are approximately 14000 lines of code).  It is split into two parts: a C++ library which implements all of the details of the simulations, and a Python package which provides a user interface to the C++ library, which is convenient to use from a Jupyter notebook.

As a guide to the C++ codebase, I have written some design notes to here: [design.md](src/design.md).  These should give a good overview of my thought processes and design considerations.

## Installation

To compile the C++ library, you will need the following libraries (all available in the Ubuntu 21.10 package repository, except for Eigen 3.4 which must be installed manually):

- [CMake v3.18](https://cmake.org/) or higher
- [Boost 1.74](https://www.boost.org/) or higher
- [Eigen 3.4](https://eigen.tuxfamily.org/index.php?title=Main_Page) or higher
- [fmtlib 7.1](https://fmt.dev/latest/index.html) or higher
- [Catch2 v2.13.9](https://github.com/catchorg/Catch2/tree/v2.13.9) (exact version only; the latest 3.x.x introduces breaking changes)

Then simply follow the usual procedure to compile a CMake project.

To install the Python package, you will need everything in `requirements.txt`, in particular [scikit-build](https://scikit-build.readthedocs.io/en/latest/index.html), which drives the build process for Cython and C++ extensions.  Install these packages and then run

    pip install .

## Usage notes

First

```py
import lennardjonesium as lj
```

to import the Python package.  It provides the functions `run()` for running a single simulation, and `run_sweep()` for automatically running many simulations over a range of temperatures and densities.  For some examples of usage, see

- [](notebooks/quick_demo/run.ipynb) for a single `run()`
- [](notebooks/small_system/run_experiment.ipynb) for a `run_sweep()`

In the future, I will consider writing more complete documentation about what all the parameters to these functions mean, but it is not hard to play around with them.
