/**
 * equilibrator.hpp
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

#ifndef LJ_EQUILIBRATOR_HPP
#define LJ_EQUILIBRATOR_HPP

#include <stdexcept>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/integrator.hpp>

namespace engine
{
    class EquilibrationError : public std::runtime_error
    {
        public:
            EquilibrationError(const char* s = "Could not reach equilibrium")
                : std::runtime_error(s)
            {}
    };

    class Equilibrator
    {
        /**
         * Equilibrator will handle the first phase of the simulation, which attempts to reach
         * equilibrium at the correct temperature.  Since temperature is a dependent variable in
         * the microcanonical ensemble, we cannot actually set a temperature directly; it must be
         * measured instead.  The InitialCondition attempts to build a state which is close to
         * the desired temperature by using a Maxwell distribution of velocities.  However, this
         * process is not perfect.  So, the Equilibrator evolves the system for some time, taking
         * temperature readings, and occasionally rescaling the velocities until the desired
         * temperature is reached and remains sufficiently stable for some amount of time.
         */

        public:
            /**
             * Equilibrator has a lot of parameters that govern the equilibration process.  These
             * are organized in the Parameters struct.  Their meaning is as follows:
             * 
             * The equilibration process happens over some number of time steps, using the
             * Integrator to advance time.  Every `sample_period` steps, we measure the temperature,
             * keeping a moving average of the last `sample_size` measurements.
             * 
             * Every `measurement_period` steps, we will check to see if the current measured
             * temperature is within `tolerance` of the target temperature.
             * 
             * If this check fails, then we rescale the temperature of the system.  If it succeeds,
             * then we begin a "steady state test" to check whether the value is stable.  This
             * means for an entire `steady_state_period`, the temperature should remain within
             * the given tolerance; i.e. *every* `measurement_period`, the temperature check must
             * succeed, until `steady_state_period` time steps have passed.  If any temperature
             * check fails, then we rescale the temperature and restart the "steady state test".
             * 
             * If the steady state test passes, then we consider the system to be equilibrated at
             * the target temperature, and we return the state.  If after `timeout` time steps the
             * system has not yet equilibrated, then we consider this a failure and raise an
             * exception.
             */
            
            struct Parameters
            {
                double tolerance = 0.05;
                int sample_size = 20;
                int sample_period = 5;
                int measurement_period = 200;
                int steady_state_period = 1000;
                int timeout = 5000;

                // We explicitly define a default constructor as demonstrated in this bug report:
                // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165
                Parameters() {}
            };

            Equilibrator(const Integrator& integrator, const Parameters& parameters = {});

            /**
             * Equilibrator is a parametrized SystemState operator, using the syntax
             * 
             *      state | equilibrator(temperature);
             * 
             * so we define operator() to return a SystemState::Operator.
             */
            physics::SystemState::Operator operator() (double temperature)
            {
                using S = physics::SystemState;
                return [this, temperature](S& s) -> S&
                    {return this->equilibrate_(s, temperature);};
            }
        
        private:
            physics::SystemState& equilibrate_(physics::SystemState& state, double temperature);

            const Integrator& integrator_;
            const Parameters& parameters_;
    };
} // namespace engine

#endif
