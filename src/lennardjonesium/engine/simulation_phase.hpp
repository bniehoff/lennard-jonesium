/**
 * simulation_phase.hpp
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

#ifndef LJ_SIMULATION_PHASE_HPP
#define LJ_SIMULATION_PHASE_HPP

#include <vector>
#include <variant>
#include <limits>

#include <lennardjonesium/tools/moving_sample.hpp>
#include <lennardjonesium/physics/measurements.hpp>

namespace engine
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
    struct RecordObservation {};

    // Adjust the temperature of the system
    struct SetTemperature
    {
        double temperature;
    };

    // On success, end this phase and move on to next
    struct PhaseComplete {};

    // On failure, end simulation
    struct AbortSimulation {};

    // The Command variant itself
    using Command = std::variant<
        std::monostate,
        RecordObservation,
        SetTemperature,
        PhaseComplete,
        AbortSimulation
    >;

    class SimulationPhase
    {
        /**
         * A SimulationPhase drives a particular phase of the simulation (e.g. equilibration or
         * observation, etc.).  The Simulator provides the SimulationPhase with the data measured
         * from the SystemState at every time step, and the SimulationPhase makes decisions about
         * whatever actions to take next (by issuing Commands).  The SimulationPhase can have
         * internal state (such as further statistical computations).
         * 
         * SimulationPhase must be given in its constructor the time step on which it will start
         * running.  In practice we will use a factory method and dynamic allocation to achieve
         * this.
         */
        public:
            // Evaluate the thermodynamic properties of the state and issue commands
            virtual Command
            evaluate(int time_step, const physics::Thermodynamics& thermodynamics) = 0;

            virtual ~SimulationPhase() = default;
        
        protected:
            const int start_time_{};

            explicit SimulationPhase(int start_time) : start_time_{start_time} {}
    };

    class EquilibrationPhase : public SimulationPhase
    {
        /**
         * EquilibrationPhase will attempt to drive the system toward equilibrium at the desired
         * temperature.  Since temperature is a dependent variable in the microcanonical ensemble,
         * we cannot actually set a temperature directly; it must be measured instead.  The
         * InitialCondition attempts to build a state which is close to the desired temperature
         * by using a Maxwell distribution of velocities.  However, this process is not perfect.
         * So, the EquilibrationPhase monitors the system for some time, taking
         * temperature readings, and occasionally rescaling the velocities until the desired
         * temperature is reached and remains sufficiently stable for some amount of time.
         */

        public:
            /**
             * EquilibrationPhase has a lot of parameters that govern the equilibration process.
             * These are organized in the Parameters struct.  Their meaning is as follows:
             * 
             * tolerance:  The allowed relative error between the system temperature and the
             *      target temperature.
             * 
             * sample_size:  The number of recent temperature measurements to use when estimating
             *      the system temperature.
             * 
             * assessment_interval:  The number of time steps after which to estimate the system
             *      temperature and make a decision.  If the temperature falls outside the
             *      tolerance range from the target temperature, then we issue a Command to rescale
             *      the system temperature.
             * 
             * steady_state_time:  If we pass this number of time steps without having to adjust
             *      the system temperature, then we consider the system to be in equilibrium at the
             *      target temperature, and we can exit the equilibration phase.
             * 
             * timeout:  If we pass this number of time steps without reaching equilibrium, then we
             *      determine that the system cannot equilibrate and we abort the simulation.
             */
            
            struct Parameters
            {
                double tolerance = 0.05;
                int sample_size = 50;
                int assessment_interval = 200;
                int steady_state_time = 1000;
                int timeout = 5000;

                // We explicitly define a default constructor as demonstrated in this bug report:
                // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165
                Parameters() {}
            };

            // The parameters will use the above defaults if not given
            EquilibrationPhase(
                int start_time,
                double target_temperature,
                Parameters parameters = {}
            )
                : SimulationPhase{start_time},
                  temperatures_(parameters.sample_size),
                  target_temperature_{target_temperature},
                  parameters_{parameters},
                  last_assessment_time_{start_time},
                  last_adjustment_time_{start_time}
            {}

            virtual Command
            evaluate(int time_step, const physics::Thermodynamics& thermodynamics) override;
        
        private:
            tools::MovingSample<double> temperatures_;
            const double target_temperature_;
            const Parameters parameters_;
            double last_mean_temperature_{std::numeric_limits<double>::signaling_NaN()};
            int last_assessment_time_;
            int last_adjustment_time_;
    };

    class ObservationPhase : public SimulationPhase
    {};
} // namespace engine


#endif
