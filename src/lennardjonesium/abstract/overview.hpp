/**
 * overview.hpp
 * 
 * Copyright (c) 2021-2022 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jonesium.
 * 
 * Lennard-Jonesium is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jonesium is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jonesium.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_OVERVIEW_HPP
#define LJ_OVERVIEW_HPP

/**
 * In this file we will put an overview of various abstract classes that make up the system, so
 * that we can see how it is all put together in one place.  This header should not be included
 * anywhere, it is just for documentation.
 * 
 * Note that we give examples of constructors for most classes, just to help understand what kinds
 * of data need to be used by whom.  But these are abstract types, and constructors should be given
 * in the concrete derived types.
 * 
 * TODO: Turn this into a Markdown file once we have a better idea of everything.
 */

#include <functional>
#include <concepts>
#include <type_traits>
#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <variant>
#include <queue>
#include <mutex>
#include <condition_variable>

#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>

/**
 * Namespaces:
 * 
 * Given in order of dependency (lower namespaces are allowed to include things from higher ones):
 * 
 * tools:   Small utility classes/functions which will be used everywhere.
 * 
 * physics: Structures related to defining physical quantities, but not the details of how these
 *          physics are simulated.  Should be given in terms that a physicist would understand.
 *          Quantities include the system state (as a collection of particles with positions,
 *          velocities, etc.), forces that act on the system or between particles, thermodynamical
 *          quantities that will be measured, etc.
 * 
 * engine:  Classes/functions related to simulating the physics; i.e., all the "hard work" involved
 *          in practically calculating the forces, integrating the equations of motion to update
 *          the system state, etc.
 * 
 * control: Structures related to simulation on a higher level: The main simulation loop, the
 *          equilibration and observation phases, etc.
 */

namespace tools
{
    class BoundingBox
    {
        /**
         * BoundingBox will describe the size of the simulation box in three dimensions.  It is
         * just a thin wrapper around an Eigen::Vector4d which gives some convenient constructors
         * and helps encapsulate the 4d vectors being used to represent the system, despite it
         * being only 3d.
         * 
         * Note that BoundingBox is always used _multiplicatively_ (e.g., to check whether something
         * is in the box, one can use `position.array() / box.array()`).  So, the 4th component of
         * the array representation is always 1.0.
         */

        public:
            explicit BoundingBox(double side_length);
            BoundingBox(double x, double y, double z);

            Eigen::Array4d array() const;
    };

    /**
     * A CellList is used to keep track of which particles are in a particular cell, or rectilinear
     * subregion of the simulation box.
     */
    using CellList = std::vector<int>;

    struct CellListPair
    {
        /**
         * CellListPair contains references to two adjacent cells, as well as an array that gives
         * the coordinates of the lattice image of the periodic bounding box to which the second
         * CellList belongs (this is to account for "neighboring cells" which wrap around the
         * boundary).  So, e.g., if lattice_image is {0, 0, 1, 0}, then the second CellList should
         * be interpreted as belonging to a copy of the simulation box that is displaced in the
         * z direction by 1 (multiple of the box's z dimension).
         */

        Eigen::Array4i lattice_image;

        CellList& first;
        CellList& second;
    };

    class CellListArray
    {
        /**
         * CellListArray maintains a structure of Cell Lists which are used to find pairs of
         * particles that are within the cutoff distance of each other.  A CellList is a list
         * of indices into the SystemState.
         * 
         * Note that we do not depend on the SystemState, and CellListArray is responsible only
         * for maintaining the CellLists and not for populating them in the first place.  The only
         * information needed is the shape (i.e. integer dimensions) of the multidimensional array
         * of cells, which can be deduced from the cutoff distance and the full dimensions of the
         * simulation box.
         */

        public:
            CellListArray(const BoundingBox& bounding_box, double cutoff_distance);

            // Access an element
            CellList& operator() (int, int, int);

            // Access an element on a const object (not sure if needed)
            const CellList& operator() (int, int, int) const;

            // Clear the elements of the array
            void clear();

            // Generator to traverse the individual cells
            std::generator<CellList&> cells() const;

            // Generator to traverse adjacent pairs of cells (including periodic wrap around)
            std::generator<CellListPair> adjacent_pairs() const;
    };

    class CubicLattice
    {
        /**
         * CubicLattice provides the coordinates of points in a cubic lattice, at the requested
         * density.  It also computes the BoundingBox of the occupied volume.  There will be
         * derived classes which implement simple, body-centered, and face-centered cubic lattices.
         */

        public:
            CubicLattice(int particle_count, double density);

            virtual std::generator<Eigen::Vector4d> operator() () = 0;

            BoundingBox bounding_box();
    };

    template<class T, class Alloc = std::allocator<T>>
    class MovingSample {
        /**
         * MovingSample keeps a fixed-sized sample of a quantity over time, used for computing
         * estimates of its statistics.  It does this by pushing values into a FIFO queue of fixed
         * length.  When requested, it computes statistics on these values (mean, variance).  These
         * statistics are sample estimates.
         * 
         * We also provide a means for clearing and restarting the queue, as well as for testing
         * whether it is completely full.
         */

        public:
            struct Statistics
            {
                T mean;
                T variance;
            };

            void push_back(T);
            void clear();
            bool full();
            Statistics statistics();
    }

    template<class T>
    class MessageBuffer
    {
        /**
         * MessageBuffer allows two threads to communicate in a single-producer, single-consumer
         * configuration, using condition variables and mutex locks to protect the buffer during
         * operations on it.
         */

        public:
            /**
             * We hide the locking mechanisms behind this interface.  The Producer should use
             * put(T) to place an item in the buffer, and call end() when it has no more items to
             * place (this allows the Consumer to know when the Producer is finished).  The Consumer
             * should get() to obtain an item from the buffer.  The call to get() will block until
             * an item is obtained; or, if the Producer has called end(), then get() will return
             * std::nullopt.  In this way, the Consumer can deduce that the buffer is empty and will
             * receive no further input, so that the Consumer can terminate.
             */
            
            void put(T);
            void end();
            std::optional<T> get();
    };
} // namespace tools

namespace physics
{
    struct SystemState
    {
        /**
         * SystemState will contain all the information about the particles in the system:
         * 
         *  positions
         *  velocities
         *  forces/accelerations
         *  total displacements (disregarding boundary conditions)
         *  kinetic energy
         *  potential energy
         *  virial
         *  (kinetic energy tensor)
         *  (virial tensor)
         * 
         * The last two are not necessary for the most basic simulator, but may be interesting in
         * the future for measuring things like shear stress and pressure.
         * 
         * SystemState only contains this information, and doesn't do anything with it.  It is
         * acted upon by Operators.
         */

        // An Operator is a function that acts on the SystemState
        using Operator = std::function<SystemState& (SystemState&)>;

        // Construct a zero-initialized state with a given number of particles
        explicit SystemState(int particle_count = 0);

        // Get the particle count if needed
        int particle_count() const;

        // Clear the dynamical quantities (forces, potential, virial)
        void clear_dynamical_quantities();
    };

    struct ForceContribution
    {
        /**
         * ForceContribution packages together the data that are needed to update the SystemState
         * when the force on a particle is calculated.  This includes not just the force value
         * itself, but also contributions to the potential energy and virial (and their tensors,
         * if those are later added).
         * 
         * This is more convenient than using a std::tuple of these quantities, because using a
         * struct allows us to name the fields.
         */

        Eigen::Vector4d force;

        double potential;
        double virial;
    };

    class ShortRangeForce
    {
        /**
         * ShortRangeForce describes a pairwise interparticle force that has a fixed range of
         * interaction; i.e., a cutoff distance, beyond which the force will not be felt.
         * 
         * ShortRangeForce is responsible for computing the ForceContribution between two particles
         * given their separation vector.
         */

        public:
            // Compute a ForceContribution from a separation vector
            virtual ForceContribution&
            operator() (const Eigen::Ref<const Eigen::Vector4d>& separation) const = 0;

            // Get the cutoff distance
            virtual double cutoff_distance() const = 0;
    };

    /**
     * One can imagine other types of forces, but we probably won't have a need for them.  They
     * should all have an operator() that returns a ForceContribution, although they may take
     * different arguments.
     */
    class LongRangeForce;
    class BackgroundForce;

    class BackgroundForce
    {
        /**
         * BackgroundForce describes a force that acts on all particles in the system.  We only
         * allow forces which are a function of position and not velocity (otherwise some
         * integration methods will become invalid).  So magnetic forces are excluded.
         */

        public:
            virtual ForceContribution&
            operator() (const Eigen::Ref<const Eigen::Vector4d>& position) const = 0;
    };

    /**
     * We also define some various Properties, which are functions that read the SystemState and
     * compute various values of physical interest.
     */
    double kinetic_energy(const SystemState&);
    Eigen::Vector4d total_momentum(const SystemState&);
    Eigen::Vector4d total_force(const SystemState&);
    Eigen::Vector4d center_of_mass(const SystemState&);
    Eigen::Vector4d total_angular_momentum(const SystemState&);
    Eigen::Matrix4d inertia_tensor(const SystemState&);

    /**
     * We also define some Measurement classes for computing (instanenous) quantities of physical
     * interest.  These quantities require combining the results of several Properties
     */

    class ThermodynamicMeasurement
    {
        public:
            // Takes the measurements and populates the internal fields
            const SystemState& operator() (const SystemState& state);

            // Get the properties measured
            double time() const;
            double kinetic_energy() const;
            double potential_energy() const;
            double total_energy() const;
            double virial() const;
            double temperature() const;
            double mean_square_displacement() const;
    };

    // This one will be related to bulk conservation laws
    class KinematicMeasurement;
} // namespace physics

namespace engine
{
    struct ParticlePair
    {
        /**
         * ParticlePair is the collection of information needed to evaluate the ShortRangeForce on
         * a pair of particles.  It contains their indices into the SystemState arrays, as well as
         * the separation vector between them (which had to be calculated already, in order to
         * filter the particle pairs by their separation distance).  The separation vector already
         * takes into account any corrections due to wrapping around the periodic boundary
         * conditions.
         */

        Eigen::Vector4d separation;

        int first;
        int second;
    };

    class ParticlePairFilter
    {
        /**
         * ParticlePairFilter is responsible for finding all the pairs of particles which are
         * within the cutoff radius of each other, for the purpose of calculating short-range
         * forces.  For example, a naive approach would be to double loop over all pairs of
         * particles and forward those whose separation distance is less than the cutoff.  In our
         * case, we will use the more sophisticated technique of Cell Lists.
         */

        public:
            ParticlePairFilter(const tools::BoundingBox& bounding_box, double cutoff_distance);

            // Generate the ParticlePairs filtered by separation distance
            // NOTE: It is _not_ const, because the process may involve using internal state
            virtual std::generator<ParticlePair> operator() (const physics::SystemState&) = 0;
    };

    class BoundaryCondition
    {
        /**
         * BoundaryCondition takes the current SystemState and enforces that all particles lie
         * within the defined BoundingBox according to the appropriate strategy for keeping them
         * there; e.g. periodic boundary conditions, reflecting boundary conditions, etc.
         */

        public:
            BoundaryCondition(const tools::BoundingBox&);

            // The BoundaryCondition is an Operator that acts on the SystemState
            virtual physics::SystemState& operator() (physics::SystemState&) const = 0;

            /**
             * We expect BoundaryCondition to be heap-allocated, since the user will choose various
             * parameters of the simulation in Python (at run time).
             */
            virtual ~BoundaryCondition() = default;
    };

    class ForceCalculation
    {
        /**
         * ForceCalculation will take the current SystemState and calculate all the forces (and
         * related dynamical quantities, such as potential energy and virial). 
         * 
         * The particular concrete implementations of ForceCalculation and BoundaryCondition are
         * not independent.  For example, a constant background force (like gravity) does not make
         * sense with periodic boundary conditons, which are intended to represent an infinite
         * system.  Even with short-range forces in a finite system, one needs to use different
         * computation methods in the case of periodic boundary conditions (infinite system) vs.
         * reflecting boundary conditions (particles trapped in a literal box).
         * 
         * To make the code fully generic while allowing runtime control over the types of problems
         * being simulated (useful if we want to provide that feature in the Python interface),
         * then we should consider implementing this "parallel dependency" via the Abstract Factory
         * pattern.  However, this is a bit complicated and is beyond the scope of this project.
         */

        public:
            ForceCalculation(const tools::BoundingBox&, const physics::ShortRangeForce&);

            // The ForceCalculation is an Operator that acts on the SystemState
            virtual physics::SystemState& operator() (physics::SystemState&) const = 0;

            /**
             * We expect ForceCalculation to be heap-allocated, since the user will choose various
             * parameters of the simulation in Python (at run time).
             */
            virtual ~ForceCalculation() = default;
    };

    class Integrator
    {
        /**
         * Integrator will use some integration strategy to increment the system by one time step.
         * Possible strategies are Euler integration, Verlet, Velocity Verlet, Symplectic Euler,
         * etc.  These will be implemented as concrete derived classes.
         * 
         * Integrator is only responsible for how position/velocity updates happen, _given_ a time
         * step and values for the forces.  Each of the above mentioned integration strategies
         * differs only in what order these updates happen in, and whether the time step is broken
         * into substeps, etc.
         */
        
        public:
            Integrator(double time_step, const BoundaryCondition&, const ForceCalculation&);

            // The integrator is an Operator that acts on the SystemState
            virtual physics::SystemState& operator() (physics::SystemState&) const = 0;

            /**
             * Returns a lambda which evolves the SystemState forward `steps` number of times.
             * The reason to return a lambda is it allows a nice way to use the piping syntax:
             * 
             *      s | integrator | integrator(n) | etc...
             */
            physics::SystemState::Operator operator() (int count) const;

            /**
             * We expect Integrator to be heap-allocated, since the user will choose various
             * parameters of the simulation in Python (at run time).
             */
            virtual ~Integrator() = default;
    };

    class InitialCondition
    {
        /**
         * InitialCondition is responsible for setting up the initial SystemState and BoundingBox.
         * This includes putting the particles in their initial positions, and giving them a
         * velocity distribution that corresponds to the desired temperature.
         */

        public:
            InitialCondition(int particle_count, double density, double temperature);

            // These methods return by value, allowing the same InitialCondition to be reused.
            tools::BoundingBox bounding_box();
            physics::SystemState system_state();
    };
} // namespace engine

namespace control
{
    /**
     * We use the Command pattern to implement communication between the SimulationPhase and the
     * Simulation.  A Command is a std::variant among the following types.  When the Simulation
     * receives a Command, it will execute the appropriate action.
     * 
     * We use std::variant and delegate the interpretation of these commands to the Simulation, so
     * that SimulationPhase does not acquire a dependency on Simulation in order to effectively
     * control it.
     */

    // Record an observation result computed from statistical data
    struct RecordObservation
    {
        physics::Observation observation;
    };

    // Adjust the temperature of the system
    struct AdjustTemperature
    {
        double temperature;
    };

    // On success, end this phase and move on to next
    struct PhaseComplete {};

    // On failure, end simulation
    struct AbortSimulation {};

    // The Command variant itself
    using Command = std::variant<
        RecordObservation,
        AdjustTemperature,
        PhaseComplete,
        AbortSimulation
    >;

    class SimulationPhase
    {
        /**
         * A Simulation::Phase drives a particular phase of the simulation (e.g. equilibration or
         * observation, etc.).  The Simulator provides the Simulation::Phase with the data measured
         * from the SystemState at every time step, and the SimulationPhase makes decisions about
         * whatever actions to take next (by issuing Commands).  The Simulation::Phase can have
         * internal state (such as further statistical computations).
         */
        public:
            // Evaluate the thermodynamic properties of the state and issue commands
            virtual std::vector<Command>
            evaluate(int time_step, const physics::ThermodynamicMeasurement& measurement) = 0;
    };

    class EquilibrationPhase : public SimulationPhase;

    class ObservationPhase : public SimulationPhase;

    // Runs the main loop, interprets commands, and creates log entries
    class Simulation
    {
        /**
         * The Simulation encapsulates the main loop, and runs the entire simulation on a given
         * initial state.  It follows a schedule of SimulationPhases which make the decisions
         * regarding what to do at each time step.  The Simulation itself is responsible for keeping
         * track of the time step count, as well as pushing relevant data to various message queues.
         */

        public:
            using Schedule = std::queue<std::unique_ptr<SimulationPhase>>;

            physics::SystemState& operator() (physics::SystemState&);

            Simulation(const engine::Integrator&, Schedule);
    };
} // namespace control


#endif
