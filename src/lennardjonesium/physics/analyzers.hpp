/**
 * analyzers.hpp
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

#ifndef LJ_ANALYZERS_HPP
#define LJ_ANALYZERS_HPP

#include <Eigen/Dense>

#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/tools/moving_sample.hpp>
#include <lennardjonesium/physics/measurements.hpp>

namespace physics
{
    /**
     * An Analyzer takes a sequence of Measurements over time and uses them to compute a physical
     * quantity of interest.  Every instant of time is a single microstate in the microcanonical
     * ensemble, so in order to get proper statistical observations, we must accumulate data over
     * several time steps.
     * 
     * It is true that in the thermodynamic limit, any typical microstate is very highly likely to
     * have collective properties (such as temperature and pressure) very close to their ensemble
     * values.  However, even in simulations with 10000 particles, we are not all that close to the
     * thermodynamic limit.
     * 
     * Some interesting quantities (such as the diffusion coefficient) actually _require_
     * measurement over time in order to evaluate (because they are related to derivatives with
     * respect to time).  The diffusion coefficient is an example of a "transport coefficient",
     * which measures the response of the system to perturbations.
     */

    template<class T>
    class Analyzer
    {
        public:
            using result_type = T;

            virtual void collect(const ThermodynamicMeasurement& measurement) = 0;
            virtual result_type result() = 0;
            virtual int sample_size() = 0;

            virtual ~Analyzer() = default;
    };

    class TemperatureAnalyzer : public Analyzer<double>
    {
        /**
         * TemperatureAnalyzer is a simplified Analyzer that only computes the temperature.  It is
         * used by the EquilibrationPhase to monitor the temperature of the simulation in order to
         * gauge whether it should be adjusted.
         */

        public:
            virtual void collect(const ThermodynamicMeasurement& measurement) override
                {temperature_sample_.push_back(measurement.temperature());}

            virtual result_type result() override
                {return temperature_sample_.statistics().mean;}
            
            virtual int sample_size() override {return sample_size_;}

            TemperatureAnalyzer
                (tools::SystemParameters system_parameters, int sample_size)
                : temperature_sample_{sample_size},
                  system_parameters_{system_parameters},
                  sample_size_{sample_size}
            {}
        
        private:
            tools::MovingSample<double> temperature_sample_;
            tools::SystemParameters system_parameters_;
            int sample_size_;
    };

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

    class ThermodynamicAnalyzer : public Analyzer<Observation>
    {
        /**
         * ThermodynamicAnalyzer collects the relevant data from a sequence of thermodynamic
         * measurements and computes an Observation which contains the quantities of physical
         * interest.
         */

        public:
            virtual void collect(const ThermodynamicMeasurement& measurement) override;
            virtual result_type result() override;
            virtual int sample_size() override {return sample_size_;}

            ThermodynamicAnalyzer
                (tools::SystemParameters system_parameters, int sample_size)
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
