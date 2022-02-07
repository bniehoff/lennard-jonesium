/**
 * observation.hpp
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

#ifndef LJ_OBSERVATION_HPP
#define LJ_OBSERVATION_HPP

#include <Eigen/Dense>

#include <lennardjonesium/tools/moving_sample.hpp>

namespace physics
{
    struct Observation
    {
        /**
         * An Observation collects together the main physical quantities that constitute the
         * "result" of the experiment.
         */

        double temperature;
        double energy;
        double pressure;
        double specific_heat;
        double diffusion_coefficient;
    };

    /**
     * We leave Observation with the trivial constructor; however, it is useful to define a
     * factory object that actually computes these quantities from the ones measured in the
     * simulation.
     */

    class ObservationFactory
    {
        public:
            ObservationFactory(double volume, int particle_count)
                : volume_{volume}, particle_count_{particle_count}
            {}

            Observation compute_observation(
                const tools::MovingSample<double>& temperatures,
                const tools::MovingSample<double>& energies,
                const tools::MovingSample<double>& virials,
                const tools::MovingSample<Eigen::Vector2d>& msd_vs_time
            );
        
        private:
            double volume_;
            int particle_count_;
    };
} // namespace physics


#endif
