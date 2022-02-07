/**
 * system_parameters.hpp
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

#ifndef LJ_SYSTEM_PARAMETERS_HPP
#define LJ_SYSTEM_PARAMETERS_HPP

namespace tools
{
    struct SystemParameters
    {
        /**
         * SystemParameters is a convenient container to keep together the relevant data that
         * describe the system to be simulated.  There are numerous constructors that need these
         * parameters, so it is useful to put them together into a simple POD struct.
         */

        double temperature;
        double density;
        int particle_count;
    };
} // namespace tools


#endif
