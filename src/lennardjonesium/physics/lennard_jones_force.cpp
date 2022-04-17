/**
 * lennard_jones_force.cpp
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

#include <cassert>
#include <cmath>

#include <Eigen/Dense>

#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/physics/lennard_jones_force.hpp>

namespace physics
{
    LennardJonesForce::LennardJonesForce(LennardJonesForce::Parameters parameters)
        : cutoff_distance_{parameters.cutoff_distance},
          square_cutoff_distance_{parameters.cutoff_distance * parameters.cutoff_distance}
    {
        /**
         * The minimum of the unmodified potential sits at
         * 
         *      r^(-6) = 1/2,   or   r^2 = 2^(1/3)
         * 
         * Let's verify that the proposed cutoff distance is larger than that.
         */

        assert(square_cutoff_distance_ > std::cbrt(2.0) && "Cutoff distance is too short");

        /**
         * Now we need to compute the spline parameters.  Refer to the comments for the compute()
         * method below.  The spline parameters must be chosen so that the potential
         * 
         *      V(r) = 4 * r^(-6) (r^(-6) - 1) + S(r)
         * 
         *      S(r) = alpha + beta * ((r/r_c)^2 - 1)
         * 
         * and virial
         * 
         *      W(r) = -r V'(r) = 24 * r^(-6) (2 * r^(-6) - 1) - r S'(r)
         * 
         *      r S'(r) = 2 * beta * (r/r_c)^2
         * 
         * both vanish at r = r_c.  This means we must have
         * 
         *      alpha = -4 * r_c^(-6) (r_c^(-6) - 1)
         * 
         *      beta = 12 * r_c^(-6) (2 * r_c^(-6) - 1)
         */

        double r_c_to_minus_6 = 1 / (
            square_cutoff_distance_ * square_cutoff_distance_ * square_cutoff_distance_
        );

        spline_alpha_ = -4.0 * r_c_to_minus_6 * (r_c_to_minus_6 - 1.0);
        spline_beta_ = 12.0 * r_c_to_minus_6 * (2.0 * r_c_to_minus_6 - 1.0);
    }

    ForceContribution
    LennardJonesForce::compute(const Eigen::Ref<const Eigen::Vector4d>& separation) const
    {
        // We will use the squared norm of the separation vector
        double r_squared = separation.squaredNorm();

        if (r_squared < square_cutoff_distance_)
        {
            /**
             * The potential is given by
             * 
             *      V(r) = 4 * strength * r^(-6) (r^(-6) - 1) + S(r)
             * 
             *      S(r) = alpha + beta * ((r/r_c)^2 - 1)
             * 
             * which we can compute from the square distance.  The virial is given by -r times the
             * derivative of the potential:
             * 
             *      W(r) = -r V'(r) = 24 * strength * r^(-6) (2 * r^(-6) - 1) - r S'(r)
             * 
             *      r S'(r) = 2 * beta * (r/r_c)^2
             * 
             * which also only requires the square distance.  Finally, the force is just the virial
             * times (\vec r)/r^2, which again just uses the square distance and the original
             * separation vector:
             * 
             *      F(r) = W(r) (\vec r) / r^2
             * 
             * Since every calculation uses the squared norm, we never have to take a square root.
             */

            double r_to_minus_6 = 1.0 / (r_squared * r_squared * r_squared);

            double potential = (
                4.0 * r_to_minus_6 * (r_to_minus_6 - 1.0)
                + spline_alpha_ + spline_beta_ * (r_squared / square_cutoff_distance_ - 1.0)
            );

            double virial = (
                24.0 * r_to_minus_6 * (2.0 * r_to_minus_6 - 1.0)
                - 2.0 * spline_beta_ * (r_squared / square_cutoff_distance_)
            );

            auto force = virial * separation / r_squared;

            return {force, potential, virial};
        }
        else
        {
            // All contributions are zero
            return {Eigen::Vector4d::Zero(), 0.0, 0.0};
        }
    }
} // namespace physics

