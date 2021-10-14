# Redesign

To be merged with main design file once we figure it out.

## Problems

We first attempted to define a Particle class with properties like position, velocity, etc.  But the
problem is, a Particle doesn't make much sense as an "object", as it doesn't have any invariants
it needs to enforce (except the relationship between position and total displacement).  It is more
of a "data structure", or something that is defined in terms of what data it stores rather than
what methods it offers.  The most important invariant with regard to Particle data is the
enforcement of boundary conditions, which is more the responsibility of some container object rather
than of the Partical itself (I cannot imagine that a Particle class should have to be told what
kind of boundary conditions are being imposed on it).

So it would be better not to have a Particle class, but instead think of Particles as just some
collection of data.  It would be more convenient to have all of our Particles in a single array
anyway, which is essentially how things were done in the original C version.

If we have arrays (or Eigen Matrices) of Positions, Velocities, Accelerations, and Displacements,
then we can very naturally use Eigen's vectorization features to write most of the Integrator in
just a few lines which update the whole arrays at once.  Then a Particle is just an index which
labels a particular column of these arrays.  (Caveat:  If we wanted to use *reflecting* boundary
conditions, then every Particle would also need to keep a history of its reflections in order to
enforce the appropriate relationship between Position and total Displacement.  This could be, e.g.,
a transformation matrix which relates position deltas to displacement deltas.)

## Solutions

Let's think of everything the simulation has to *do*, and then afterward think about how these tasks
might be divided between distinct units.

1. Update positions and velocities based on velocities and accelerations.
2. Update forces (requires cell lists).
3. Impose boundary conditions.
4. A way to take a moving average of particle data in order to track important physics.
5. A way to kick the system with velocity rescalings until we reach the desired temperature.
6. A way to log relevant data to a file (thermo measurements and maybe particle trajectories)
7. Maybe a way to parallelize for very large simulations.

We want to separate concerns as much as possible:

- Integrator: Will handle Velocity-Verlet updates to positions and velocities
- Force Law
