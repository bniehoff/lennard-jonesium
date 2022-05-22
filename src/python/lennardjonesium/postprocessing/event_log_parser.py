"""
event_log_parser.py

Copyright (c) 2021-2022 Benjamin E. Niehoff

This file is part of Lennard-Jonesium.

Lennard-Jonesium is free software: you can redistribute
it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

Lennard-Jonesium is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with Lennard-Jonesium.  If not, see
<https://www.gnu.org/licenses/>.
"""


from enum import Enum, auto
from typing import Optional
import pathlib


class SimulationStatus(Enum):
    completed = auto()
    equilibration_aborted = auto()
    observation_aborted = auto()


class EventLogParser:
    """
    EventLogParser reads event log files and gathers information about what happened during a
    simulation, such as:

        - Whether it completed or failed
        - If it failed, during which stage
        - At what time step did the observation phase start?
        - How many temperature adjustments were required?  On which time steps?
        - How many observations were recorded?  On which time steps?
    
    This will allow us to easily gather this information for a large collection of files.
    """

    simulation_status: Optional[SimulationStatus] = None

    equilibration_started: Optional[int] = None
    equilibration_completed: Optional[int] = None
    equilibration_time_steps: Optional[int] = None

    observation_started: Optional[int] = None
    observation_completed: Optional[int] = None
    observation_time_steps: Optional[int] = None

    total_time_steps: int = 0

    temperature_adjustments: list[int]
    observations_recorded: list[int]

    def __init__(self, event_log: pathlib.Path) -> None:
        self.temperature_adjustments = []
        self.observations_recorded = []
        
        with open(event_log, 'r') as event_log_file:
            self._parse(event_log_file.readlines())

    def _parse(self, lines: list[str]):
        """
        Parses the event log entries for the information we want.  Fortunately this is simple and
        can be done easily enough without regular expressions.
        """
        for line in lines:
            # First separate the time step from the rest of the line
            first, rest = line.split(': ', maxsplit=1)
            time_step = int(first)

            self.total_time_steps = time_step

            # Look for phase started
            if rest.startswith('Phase started'):
                if self.equilibration_started is None:
                    self.equilibration_started = time_step
                else:
                    self.observation_started = time_step
                continue

            # Look for phase complete
            if rest.startswith('Phase complete'):
                if self.equilibration_completed is None:
                    self.equilibration_completed = time_step
                    self.equilibration_time_steps = time_step - self.equilibration_started
                else:
                    self.observation_completed = time_step
                    self.observation_time_steps = time_step - self.observation_started
                    # At this point, we also know that the simulation completed successfully
                    self.simulation_status = SimulationStatus.completed
                continue

            # Temperature adjustments
            if rest.startswith('Temperature'):
                self.temperature_adjustments.append(time_step)
                continue

            # Observations
            if rest.startswith('Observation'):
                self.observations_recorded.append(time_step)
                continue
            
            # Whether simulation finished
            if rest.startswith('Simulation aborted'):
                if self.equilibration_completed is None:
                    self.simulation_status = SimulationStatus.equilibration_aborted
                else:
                    self.simulation_status = SimulationStatus.observation_aborted
