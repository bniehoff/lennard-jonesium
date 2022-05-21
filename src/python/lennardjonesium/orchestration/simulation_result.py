"""
simulation_result.py

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
import pathlib


from lennardjonesium.tools import last_line
from lennardjonesium.simulation import Configuration


class SimulationResult(Enum):
    completed = auto()
    equilibration_aborted = auto()
    observation_aborted = auto()


def get_simulation_result(config_file: pathlib.Path) -> SimulationResult:
    """
    Gets the result of a completed simulation (whether it actually completed, or aborted due to
    inability to equilibrate, or inability to maintain a stable temperature during observation).

    Must use a config file rather than a Configuration object so that it has a point of reference
    from which to find the path to the events log file.  If the events log does not exist, then
    we raise an error.
    """
    cfg = Configuration.from_file(config_file)

    events_log = pathlib.Path(cfg.filepaths.event_log)
    if not events_log.is_absolute():
        events_log = config_file.parent / events_log
    
    if not events_log.is_file():
        raise FileNotFoundError('Requested simulation has not generated events log')
    
    result_line = last_line(events_log)

    if 'aborted' in result_line:
        if 'equilibrate' in result_line:
            return SimulationResult.equilibration_aborted
        else:
            return SimulationResult.observation_aborted
    else:
        return SimulationResult.completed
