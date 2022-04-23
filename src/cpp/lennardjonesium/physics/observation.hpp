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

/**
 * This file is very short, but we needed to separate this dependency from others
 */

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
} // namespace physics


#endif
