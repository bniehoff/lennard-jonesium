"""
sweep_result.py

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


from dataclasses import dataclass
import pathlib


from lennardjonesium.simulation import Configuration
from lennardjonesium.orchestration.run_result import SimulationStatus, RunResult
from lennardjonesium.orchestration.sweep_configuration import SweepConfiguration


class SweepResult:
    """
    SweepResult collects together all the useful high-level information about the result of running
    a sweep of simulations.  For example, how many simulations completed, or aborted for various
    reasons, and of those which were aborted, how far did they get into their observation run.
    """
    
    @dataclass
    class _SimulationResult:
        simulation_dir: pathlib.Path
        event_data: RunResult
    
    completed: list[_SimulationResult]
    equilibration_aborted: list[_SimulationResult]
    observation_aborted: list[_SimulationResult]

    def __init__(self, sweep_config_file: pathlib.Path) -> None:
        self.completed = []
        self.equilibration_aborted = []
        self.observation_aborted = []

        self._collect_results(sweep_config_file)
    
    def _collect_results(self, sweep_config_file: pathlib.Path):
        """
        Collect the results from all the event logs that were generated in this simulation sweep.
        """
        sweep_dir = sweep_config_file.parent
        sweep_cfg = SweepConfiguration.from_file(sweep_config_file)

        for simulation_dir in sweep_cfg.simulation_dir_range():
            run_config_file = sweep_dir / simulation_dir / sweep_cfg.templates.run_config_file
            
            run_result = RunResult(run_config_file)

            if run_result.simulation_status == SimulationStatus.completed:
                category = self.completed
            elif run_result.simulation_status == SimulationStatus.equilibration_aborted:
                category = self.equilibration_aborted
            elif run_result.simulation_status == SimulationStatus.observation_aborted:
                category = self.observation_aborted
            
            category.append(self._SimulationResult(simulation_dir, run_result))
