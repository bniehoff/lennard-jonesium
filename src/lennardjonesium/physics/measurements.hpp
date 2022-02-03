/**
 * measurements.hpp
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

#ifndef LJ_MEASUREMENTS_HPP
#define LJ_MEASUREMENTS_HPP

#include <cassert>

#include <Eigen/Dense>

#include <lennardjonesium/physics/system_state.hpp>

namespace physics
{
    /**
     * First we define several useful measurement _functions_ which look at the SystemState and
     * compute some value.  These are not the same as the Measurement _concept_, which is supposed
     * to return a const reference to the SystemState so that multiple Measurements can be chained
     * together.  However, these functions are important building blocks for making Measurements.
     */

    double kinetic_energy(const SystemState&);

    double mean_square_displacement(const SystemState&);

    Eigen::Vector4d total_momentum(const SystemState&);

    Eigen::Vector4d total_force(const SystemState&);

    Eigen::Vector4d center_of_mass(const SystemState&);

    Eigen::Vector4d total_angular_momentum
        (const SystemState& state, const Eigen::Ref<const Eigen::Vector4d>& center);
        
    inline Eigen::Vector4d total_angular_momentum(const SystemState& state)
        {return total_angular_momentum(state, Eigen::Vector4d::Zero());}

    /**
     * Since the total energy requires the kinetic energy to be computed, we provide an overload
     * which can accept a pre-computed value, in order not to waste effort.
     */
    inline double total_energy(const SystemState& state, double kinetic_energy)
        {return state.potential_energy + kinetic_energy;}
    
    inline double total_energy(const SystemState& state)
        {return total_energy(state, kinetic_energy(state));}
    
    /**
     * Similarly, the temperature is proportional to average the kinetic energy, so we provide
     * simple overloads.
     */
    inline double temperature(const SystemState& state, double kinetic_energy)
    {
        assert(state.particle_count() > 0 && "Cannot compute temperature of empty state.");
        
        return (2./3.) * kinetic_energy / static_cast<double>(state.particle_count());
    }

    inline double temperature(const SystemState& state)
        {return temperature(state, kinetic_energy(state));}
    
    /**
     * The inertia tensor is given as a 4x4 matrix for alignement reasons.  The upper 3x3 block
     * is the 3-dimensional inertia tensor.  The (i, 4) and (4, j) elements are zero.  The (4, 4)
     * element is equal to 1/2 times the trace of the 3-dimensional inertia tensor, although its
     * actual value should never come into play, since 3d vectors are represented as 4d vectors
     * with a 0 as their 4th component.
     * 
     * Note also that the inertia tensor can have zero eigenvalues!  This happens only if all of
     * the particles are collinear with each other and with the center point.  In most practical
     * situations, this will not happen, but in general one must take care when inverting the
     * inertia tensor.
     */

    Eigen::Matrix4d inertia_tensor
        (const SystemState& state, const Eigen::Ref<const Eigen::Vector4d>& center);

    inline Eigen::Matrix4d inertia_tensor(const SystemState& state)
        {return inertia_tensor(state, Eigen::Vector4d::Zero());}
    
    /**
     * Now we define some Measurement objects which bundle certain useful combination of measurement
     * functions together.
     */

    class Thermodynamics
    {
        // A Measurement which gathers thermodynamic information about the state
        public:
            const SystemState& operator() (const SystemState&);

            double kinetic_energy() const {return kinetic_energy_;}
            double potential_energy() const {return potential_energy_;}
            double total_energy() const {return total_energy_;}
            double virial() const {return virial_;}
            double temperature() const {return temperature_;}
            double mean_square_displacement() const {return mean_square_displacement_;}

        private:
            double kinetic_energy_;
            double potential_energy_;
            double total_energy_;
            double virial_;
            double temperature_;
            double mean_square_displacement_;
    };

    class Diagnostics
    {
        // A Measurement which gathers information about conservation laws which may be useful
        public:
            const SystemState& operator() (const SystemState&);

            Eigen::Vector4d total_momentum() {return total_momentum_;}
            Eigen::Vector4d total_force() {return total_force_;}
            Eigen::Vector4d center_of_mass() {return center_of_mass_;}
            Eigen::Vector4d total_angular_momentum() {return total_angular_momentum_;}
        
        private:
            Eigen::Vector4d total_momentum_;
            Eigen::Vector4d total_force_;
            Eigen::Vector4d center_of_mass_;
            Eigen::Vector4d total_angular_momentum_;
    };
} // namespace physics

#endif
