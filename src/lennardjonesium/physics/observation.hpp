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

#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/tools/moving_sample.hpp>
#include <lennardjonesium/physics/measurements.hpp>

namespace physics
{
    struct Observation
    {
        /**
         * An Observation collects together the main physical quantities that constitute the
         * "result" of the experiment.
         */

        double temperature;
        double pressure;
        double specific_heat;
        double diffusion_coefficient;
    };

    class ObservationComputer
    {
        /**
         * Collects the data (i.e. ThermodynamicSnapshots) needed to compute an Observation, and
         * then does the statistical computations when requested.
         */

        public:
            Observation compute();

            void collect_data(const ThermodynamicSnapshot& snapshot);

            int sample_size() {return sample_size_;}

            ObservationComputer(tools::SystemParameters system_parameters, int sample_size)
                : temperature_sample_{sample_size},
                  virial_sample_{sample_size},
                  msd_vs_time_sample_{sample_size},
                  system_parameters_{system_parameters},
                  sample_size_{sample_size}
            {}

        private:
            tools::MovingSample<double> temperature_sample_;
            tools::MovingSample<double> virial_sample_;
            tools::MovingSample<Eigen::Vector2d> msd_vs_time_sample_;
            tools::SystemParameters system_parameters_;
            int sample_size_;
    };
} // namespace physics


#endif
