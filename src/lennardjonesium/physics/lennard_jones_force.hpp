/**
 * lennard_jones_force.hpp
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

#ifndef LJ_LENNARD_JONES_FORCE_HPP
#define LJ_LENNARD_JONES_FORCE_HPP

#include <Eigen/Dense>

#include <lennardjonesium/physics/forces.hpp>

namespace physics
{
    class LennardJonesForce : public ShortRangeForce
    {
        /**
         * The LennardJonesForce is the short-range force on which the main simulation is based.
         * It is obtained from an interparticle potential of the form
         * 
         *      V(r) = 4 * epsilon * (r^(-12) - r^(-6))
         * 
         * where r is the separation distance.  In order to cut off the force at a finite distance,
         * we add a spline function S(r)
         * 
         *      V(r) = 4 * epsilon * (r^(-12) - r^(-6)) + S(r)
         * 
         * which takes the potential smoothly to zero at some fixed cutoff distance.  The spline
         * function is quadratic, and takes the form
         * 
         *      S(r) = alpha + beta * ((r/r_c)^2 - 1)
         * 
         * where r_c is the cutoff distance, and alpha and beta are some parameters which are
         * chosen to meet the desired conditions at r = r_c.
         */

        public:
            struct Parameters
            {
                /**
                 * The cutoff distance is the only parameter needed, since the overall strength
                 * merely sets the temperature scale for the dimensionless variables.
                 * 
                 * We still wrap this in a Parameters struct, because this part of the interface
                 * allows us to inform the Simulation wrapper what parameters are needed.
                 */
                double cutoff_distance = 2.5;
            };

            explicit LennardJonesForce(Parameters parameters);
            
            LennardJonesForce() : LennardJonesForce(Parameters{}) {}

            // Compute a ForceContribution from a separation vector
            virtual ForceContribution
            compute(const Eigen::Ref<const Eigen::Vector4d>& separation) const override;

            // Get the cutoff distance
            virtual double cutoff_distance() const override {return cutoff_distance_;}
        
        private:
            double cutoff_distance_;
            double square_cutoff_distance_;
            double spline_alpha_;
            double spline_beta_;
    };
} // namespace physics


#endif
