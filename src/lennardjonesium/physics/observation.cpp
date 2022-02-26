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

#include <Eigen/Dense>

#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/tools/moving_sample.hpp>
#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/physics/observation.hpp>

namespace physics
{
    void ObservationComputer::collect_data(const ThermalMeasurement& snapshot)
    {
        temperature_sample_.push_back(snapshot.temperature());
        virial_sample_.push_back(snapshot.virial());

        // The mean square displacement as a function of time will be used for linear regression
        msd_vs_time_sample_.push_back({snapshot.time(), snapshot.mean_square_displacement()});
    }

    Observation ObservationComputer::compute()
    {
        /**
         * Compute physical quantities to package into an Observation.
         */
        auto temperature_statistics = temperature_sample_.statistics();
        auto virial_statistics = virial_sample_.statistics();
        auto msd_vs_time_statistics = msd_vs_time_sample_.statistics();

        // The temperature is simply the mean temperature:
        double temperature = temperature_statistics.mean;

        /**
         * The pressure is computed from the virial theorem:
         * 
         *      P = (N <T>)/V + <W>/(d V)
         * 
         * where N is the particle count, <T> is the temperature, V is the volume, <W> is the
         * virial, and d is the dimension (3).  Note that N/V is the density, so we can also
         * obtain the volume via
         * 
         *      V = particle_count / density
         * 
         * Therefore we can write the pressure as follows:
         * 
         *      P = (N / V) * (<T> + <W> / (3 N))
         */
        double pressure = system_parameters_.density * (
            temperature + virial_statistics.mean / (3.0 * system_parameters_.particle_count)
        );

        /**
         * To compute the specific heat capacity, we must take some care.  Simple formulas for
         * heat capacity are usually given in terms of the fluctuations of the total energy,
         * in the canonical ensemble.  However, our simulation takes place in the microcanonical
         * ensemble, and the total energy is formally constant (naturally it has some
         * fluctuation, but this can be suppressed to arbitrarily high order by an appropriate
         * choice of symplectic integrator).  So we need an appropriate formula that is well-
         * defined in the microcanonical ensemble.  The following paper derives such a formula:
         * 
         *  J. L. Lebowitz, J. K. Percus, and L. Verlet, "Ensemble Dependence of Fluctuations
         *  with Application to Machine Computations", Phys. Rev. 153, 250 – 5 January 1967,
         *  https://journals.aps.org/pr/abstract/10.1103/PhysRev.153.250
         * 
         * Or directly from J. L. Lebowitz's homepage,
         * 
         *  https://cmsr.rutgers.edu/images/people/lebowitz_joel/publications/jll.pub_46.pdf
         * 
         * Equation (3.7) states that
         * 
         *      <(dKE)^2> / N = (3 <T>^2 /2) (1 - (3/2) (C_V)^(-1))
         * 
         * where C_V is the specific heat and <(dKE)^2> is the variance of the kinetic energy.
         * Since kinetic energy is proportional to temperature,
         * 
         *      <T> = 2 <KE> / (3 N)
         * 
         * we can rewrite this as
         * 
         *      (3/2) N <(dT)^2> = <T>^2 (1 - (3/2) (C_V)^(-1))
         * 
         * with <(dT)^2> the variance of the temperature.  We can then solve for the specific
         * heat:
         * 
         *      C_V = (3/2) (1 - (3/2) N <(dT)^2> / <T>^2)^(-1)
         */
        double specific_heat = (3./2.) / (
            1 - (3./2.) * system_parameters_.particle_count * (
                temperature_statistics.variance / (temperature * temperature)
            )
        );

        /**
         * The diffusion coefficient D is related to the mean square displacement via
         * 
         *      <|r(t) - r(t_0)|^2> = 6 D (t - t_0)
         * 
         * Or, the coefficient D is 1/6 times the slope of a fitting line to the msd-vs-time curve.
         * 
         * We compute the slope of the fitting line by taking
         * 
         *      D = (1/6) * (Cov(t, MSD) / Var(t))
         * 
         * Note that in this formula, the Bessel correction factor for sample (co)variance divides
         * out, so it is irrelevant whether we use it or not.
         * 
         * Also note: For maximal numerical stability, it would be better to solve for the fit
         * line with a linear solver (such as one of the many provided by Eigen), rather than
         * directly writing the fit parameters in terms of the variance, covariance, etc. (which
         * is effectively equivalent to inverting a matrix).  However, the _slope_ of the fit line
         * is usually stable enough.  The _intercept_, by constrast, would be strongly affected
         * by the fact that both the time and MSD values are far from zero and potentially live
         * on rather different numerical scales.
         */
        double diffusion_coefficient = (1./6.) * (
            msd_vs_time_statistics.covariance(0, 1) / msd_vs_time_statistics.covariance(0, 0)
        );

        // Now assemble the return value
        return Observation{
            temperature,
            pressure,
            specific_heat,
            diffusion_coefficient
        };
    }
} // namespace physics

