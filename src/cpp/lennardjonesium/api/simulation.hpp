/**
 * simulation.hpp
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

#ifndef LJ_SIMULATION_HPP
#define LJ_SIMULATION_HPP

#include <memory>
#include <variant>
#include <random>
#include <vector>
#include <string>
#include <utility>
#include <filesystem>

#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/tools/cubic_lattice.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/physics/lennard_jones_force.hpp>
#include <lennardjonesium/engine/initial_condition.hpp>
#include <lennardjonesium/output/logger.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>
#include <lennardjonesium/control/simulation_controller.hpp>

namespace api
{
    class Simulation
    {
        /**
         * Simulation encapsulates all the information needed to run a complete simulation.  It
         * provides some simple methods for running the simulation and getting information about it:
         * 
         *  Running:
         *      run():          Run the simulation
         * 
         *  Information:
         *      parameters():   Get the parameters used to define the simulation
         * 
         *  Used for making plots:
         *      potential():    Evaluate the potential for a given separation distance
         *      virial():       Evaluate the virial for a given separation distance
         *      force():        Evaluate the force for a given separation distance
         */

        using force_parameter_type = std::variant<
            physics::LennardJonesForce::Parameters
        >;

        using simulation_phase_parameter_type = std::variant<
            control::EquilibrationPhase::Parameters,
            control::ObservationPhase::Parameters
        >;

        public:
            struct Parameters
            {
                /**
                 * The Parameters struct contains all the information needed to create the
                 * SimulationController and run the simulation.
                 */

                // Parameters describing the physical properties of the system
                tools::SystemParameters system_parameters = {
                    .temperature{0.8}, .density{1.0}, .particle_count{100}
                };

                // Unit cell type for the initial lattice
                tools::CubicLattice::UnitCell unit_cell = tools::CubicLattice::FaceCentered();

                // The random seed to use for initial state construction
                std::random_device::result_type random_seed =
                    engine::InitialCondition::random_number_engine_type::default_seed;
                
                // Parameters to configure the short range force
                force_parameter_type force_parameters = {};

                // The size of the time step to use for integration
                double time_delta = 0.005;

                // Each SimulationPhase must be given a name and a set of parameters
                std::vector<std::pair<std::string, simulation_phase_parameter_type>>
                    schedule_parameters = {
                        {"Equilibration Phase", control::EquilibrationPhase::Parameters{}},
                        {"Observation Phase", control::ObservationPhase::Parameters{}}
                    };
                
                // Filesystem defaults simply place files at top level in the working directory
                std::filesystem::path event_log_path = "events.log";
                std::filesystem::path thermodynamic_log_path = "thermodynamics.csv";
                std::filesystem::path observation_log_path = "observations.csv";
                std::filesystem::path snapshot_log_path = "snapshots.csv";
            };

            explicit Simulation(Parameters parameters);

            // Builds a SimulationController from the Parameters and runs it
            void run();

            Parameters parameters() {return parameters_;}

            // Evaluate the basic functions that describe the force.  Useful for plotting.
            double potential(double distance) {return short_range_force_->potential(distance);}
            double virial(double distance) {return short_range_force_->virial(distance);}
            double force(double distance) {return short_range_force_->force(distance);}
        
        private:
            Parameters parameters_;

            engine::InitialCondition initial_condition_;

            // We use a pointer to the generic ShortRangeForce, in case we might like to implement
            // other ShortRangeForces in the future
            std::unique_ptr<const physics::ShortRangeForce> short_range_force_;

            // Construct the SimulationController from the local parameters and a Logger
            control::SimulationController make_simulation_controller_(output::Logger&);
    };
} // namespace api


#endif
