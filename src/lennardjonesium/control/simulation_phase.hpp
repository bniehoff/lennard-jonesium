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

#include <memory>
#include <vector>
#include <variant>
#include <limits>

#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/tools/moving_sample.hpp>
#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/physics/analyzers.hpp>

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
            virtual std::vector<Command>
            evaluate(int time_step, const physics::ThermodynamicMeasurement& measurement) = 0;

            virtual ~SimulationPhase() = default;
        
        protected:
            const int start_time_{};

            explicit SimulationPhase(int start_time) : start_time_{start_time} {}
    };

    struct EquilibrationParameters
    {
        /**
         * This really ought to be a nested struct inside EquilibrationPhase below, however there
         * is a bug in GCC that prevents default-initializing it properly:
         * 
         *      https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165
         * 
         * An alternative is to provide a default constructor, but then it will not be recognized
         * as an aggregate type, and thus wouldn't allow designated initialization (which is a very
         * convenient way to define these parameters).
         * 
         * EquilibrationPhase has a lot of parameters that govern the equilibration process.
         * Their meaning is as follows:
         * 
         * tolerance:  The allowed relative error between the system temperature and the
         *      target temperature.
         * 
         * sample_size:  The number of recent temperature measurements to use when estimating
         *      the system temperature.
         * 
         * adjustment_interval:  The number of time steps after which to estimate the system
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

        double tolerance = 0.05;
        int sample_size = 50;
        int adjustment_interval = 200;
        int steady_state_time = 1000;
        int timeout = 5000;
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
            // The parameters will use the above defaults if not given
            EquilibrationPhase(
                int start_time,
                tools::SystemParameters system_parameters,
                EquilibrationParameters equilibration_parameters = {}
            )
                : SimulationPhase{start_time},
                  temperature_analyzer_(system_parameters, equilibration_parameters.sample_size),
                  system_parameters_{system_parameters},
                  equilibration_parameters_{equilibration_parameters},
                  last_adjustment_check_time_{start_time},
                  last_adjustment_time_{start_time}
            {}

            virtual std::vector<Command>
            evaluate(int time_step, const physics::ThermodynamicMeasurement& measurement) override;
        
        private:
            physics::TemperatureAnalyzer temperature_analyzer_;
            tools::SystemParameters system_parameters_;
            EquilibrationParameters equilibration_parameters_;
            double last_temperature_{std::numeric_limits<double>::signaling_NaN()};
            int last_adjustment_check_time_;
            int last_adjustment_time_;
    };

    struct ObservationParameters
    {
        /**
         * This really ought to be a nested struct inside EquilibrationPhase below, however there
         * is a bug in GCC that prevents default-initializing it properly:
         * 
         *      https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165
         * 
         * An alternative is to provide a default constructor, but then it will not be recognized
         * as an aggregate type, and thus wouldn't allow designated initialization (which is a very
         * convenient way to define these parameters).
         * 
         * The entries have the following meanings:
         * 
         * tolerance:  Since temperature is a dependent variable, even after the Equilibration
         *      phase, we cannot guarantee that the temperature will remain stable.  This
         *      parameter gives the allowed window in which the observed temperature will be
         *      considered valid; if the temperature leaves this window, the simulation will be
         *      aborted.  The value given here should probably be larger than the one used for
         *      equilibration.  Some variance in the temperature is necessary for determining
         *      the other observed quantities, such as specific heat.
         * 
         * sample_size:  The number of recent measurements to include in statistics, when making
         *      observations.
         * 
         * observation_interval:  The number of time steps to wait between making observations.
         * 
         * observation_count:  The number of observations to make; this determines the running
         *      time of the full experiment.  Note that if the temperature strays outside the
         *      given tolerance, then the experiment will be ended; however, observations made
         *      up to that point should be valid (since they were made at a temperature within
         *      the allowed tolerance.)
         */

        double tolerance = 0.10;
        int sample_size = 50;
        int observation_interval = 200;
        int observation_count = 20;
    };

    class ObservationPhase : public SimulationPhase
    {
        /**
         * The ObservationPhase passively observes the SystemState and makes periodic Observations
         * of physically relevant quantities.  These include, e.g., the temperature, total energy,
         * pressure, specific heat, and diffusion coefficient.
         */

        public:
            // The parameters will use the above defaults if not given
            ObservationPhase(
                int start_time,
                tools::SystemParameters system_parameters,
                ObservationParameters observation_parameters = {}
            )
                : SimulationPhase{start_time},
                  thermodynamic_analyzer_{system_parameters, observation_parameters.sample_size},
                  system_parameters_{system_parameters},
                  observation_parameters_{observation_parameters},
                  last_observation_time_{start_time}
            {}

            virtual std::vector<Command>
            evaluate(int time_step, const physics::ThermodynamicMeasurement& measurement) override;
        
        private:
            physics::ThermodynamicAnalyzer thermodynamic_analyzer_;
            tools::SystemParameters system_parameters_;
            ObservationParameters observation_parameters_;
            int last_observation_time_;
            int observation_count_{0};
    };

} // namespace control
#endif