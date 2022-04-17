/**
 * sinks.cpp
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

#include <ranges>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/output/log_message.hpp>
#include <lennardjonesium/output/sinks.hpp>

namespace output
{
    void EventSink::write(int time_step, PhaseStartEvent message)
    {
        fmt::print(
            destination_,
            "{}: Phase started: {}\n",
            time_step,
            message.name
        );
    }

    void EventSink::write(int time_step, AdjustTemperatureEvent message)
    {
        fmt::print(
            destination_,
            "{}: Temperature adjusted to: {}\n",
            time_step,
            message.temperature
        );
    }
    
    void EventSink::write(int time_step, RecordObservationEvent message [[maybe_unused]])
    {
        fmt::print(
            destination_,
            "{}: Observation recorded\n",
            time_step
        );
    }
    
    void EventSink::write(int time_step, PhaseCompleteEvent message)
    {
        fmt::print(
            destination_,
            "{}: Phase complete: {}\n",
            time_step,
            message.name
        );
    }
    
    void EventSink::write(int time_step, AbortSimulationEvent message)
    {
        fmt::print(
            destination_,
            "{}: Simulation aborted: {}\n",
            time_step,
            message.reason
        );
    }

    void ThermodynamicSink::write_header()
    {
        fmt::print(
            destination_,
            "{},{},{},{},{},{},{},{}\n",
            "TimeStep",
            "Time",
            "KineticEnergy",
            "PotentialEnergy",
            "TotalEnergy",
            "Virial",
            "Temperature",
            "MeanSquareDisplacement"
        );
    }

    void ThermodynamicSink::write(int time_step, ThermodynamicData message)
    {
        fmt::print(
            destination_,
            "{},{},{},{},{},{},{},{}\n",
            time_step,
            message.data.time,
            message.data.kinetic_energy,
            message.data.potential_energy,
            message.data.total_energy,
            message.data.virial,
            message.data.temperature,
            message.data.mean_square_displacement
        );
    }

    void ObservationSink::write_header()
    {
        fmt::print(
            destination_,
            "{},{},{},{},{}\n",
            "TimeStep",
            "Temperature",
            "Pressure",
            "SpecificHeat",
            "DiffusionCoefficient"
        );
    }

    void ObservationSink::write(int time_step, ObservationData message)
    {
        fmt::print(
            destination_,
            "{},{},{},{},{}\n",
            time_step,
            message.data.temperature,
            message.data.pressure,
            message.data.specific_heat,
            message.data.diffusion_coefficient
        );
    }

    void SystemSnapshotSink::write_header()
    {
        /**
         * TODO: Fix formatting for this CSV file so that the multi-index headers/index_cols are
         * imported correctly.
         */

        // We set up two header rows for a multi-index Pandas dataframe
        fmt::print(
            destination_,
            "{},{},{},{},{},{},{},{},{},{},{}\n",
            "TimeStep", "ParticleID",
            "Position", "Position", "Position",
            "Velocity", "Velocity", "Velocity",
            "Force", "Force", "Force"
        );

        fmt::print(
            destination_,
            "{},{},{},{},{},{},{},{},{},{},{}\n",
            "TimeStep", "ParticleID",
            "X", "Y", "Z",
            "X", "Y", "Z",
            "X", "Y", "Z"
        );
    }

    void SystemSnapshotSink::write(int time_step, SystemSnapshot message)
    {
        for (int particle_id : std::views::iota(0, message.positions.cols()))
        {
            fmt::print(
                destination_,
                "{},{},{},{},{},{},{},{},{},{},{}\n",
                time_step,
                particle_id,
                message.positions.col(particle_id).x(),
                message.positions.col(particle_id).y(),
                message.positions.col(particle_id).z(),
                message.velocities.col(particle_id).x(),
                message.velocities.col(particle_id).y(),
                message.velocities.col(particle_id).z(),
                message.forces.col(particle_id).x(),
                message.forces.col(particle_id).y(),
                message.forces.col(particle_id).z()
            );
        }
    }
} // namespace output

