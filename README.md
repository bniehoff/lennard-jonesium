# lennard-jonesium

Microcanonical simulation of the Lennard-Jones substance and analysis of phase transitions.
Completely rebuilt in Python and modern C++ using object-oriented and functional programming
principles.

The project is finally "done"!  The C++ part (which does all the hard work) is working well, and
the Python wrapper package finally has a usable interface.  With the Python wrapper, one can easily
`run()` a single simulation, or `run_sweep()` a whole "sweep" of simulations over parameter space.
When using `run_sweep()`, everything about the directory structure of the collection of simulations
is managed automatically.

TODO: Some more detailed instructions about installation and use are needed, of course.

To begin reading and understanding the code, I suggest starting with [design.md](src/design.md).
