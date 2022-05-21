"""
run_sweep.py

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


from calendar import c
import os
import pathlib
from dataclasses import dataclass, field
from typing import Union, Optional, Iterator
from types import FunctionType, BuiltinFunctionType
from copy import deepcopy
import itertools
import time
import textwrap

import numpy as np

from lennardjonesium.simulation import Configuration, Simulation, SimulationPool
from lennardjonesium.orchestration.sweep_configuration import SweepConfiguration
from lennardjonesium.orchestration.simulation_result import SimulationResult, get_simulation_result


@dataclass
class SweepResult:
    """
    A simple dataclass to contain the "result" of a sweep: i.e., out of all the simulations
    launched, which ones completed successfully, and which ones were aborted for some reason.
    """
    completed: list[pathlib.Path] = field(default_factory=list)
    equilibration_aborted: list[pathlib.Path] = field(default_factory=list)
    observation_aborted: list[pathlib.Path] = field(default_factory=list)


def run_sweep(
    sweep_config_file: Union[str, pathlib.Path],
    echo_status: bool = True,
    polling_interval: float = 0.5,
    thread_count: int = 4,
    random_seed: Union[None, int, FunctionType, BuiltinFunctionType] = None,
    sweep_config_object: Optional[SweepConfiguration] = None
) -> SweepResult:
    """
    Wrapper function for running many simulations over a range of parameter space.

    TODO: Define some preamble/postamble to print a description of what's happening.

    :param sweep_config_file: File path to the sweep config file which describes the simulation
        sweep to be run.  The output sweep config file will be written to the same place (possibly
        after changing the random seed).  If this file does not already exist, then the 
        sweep_config_object parameter MUST be given, and a sweep config file will be created at this
        path.  If the config file does exist, it will be overwritten, perhaps with a fresh random
        seed.

        The individual config files and output files of the simulations in the sweep will be created
        under the parent directory of the sweep config file.
    
    :param echo_status: If True, print information about the running simulation to the console.
        Otherwise the run will be silent.
    
    :param polling_interval: If echo_status is enabled, controls how often (in seconds) we poll
        the SimulationPool object in order to check on the status of the jobs and update the
        console printout.
    
    :param thread_count: The number of threads to use for running simulations in the sweep.
        A single simulation runs in its own thread (and uses a second thread for file IO), but if
        there are additional cores available, then multiple simulations can be run in parallel,
        up to the number given by thread_count.
    
    :param random_seed: Can be None, an int, or a function which takes 0 arguments.
        If None, then use the pre-existing random seed from the individual run config file, if
            it exists; otherwise use the default random seed
        If an int, then use this random seed for all simulations in the sweep.
        If a function, then call this function to obtain an individual random seed for each
            simulation in the sweep.
    
    :param sweep_config_object: An optional SweepConfiguration object.  If this is given, then it
        will be used in preference over the sweep_config_file, and the sweep_config_file will be
        overwritten with the data provided by the sweep_config_object.
    
    We will always output a config file with the random seed actually used, which will overwrite
    the original config file.
    """

    sweep_config_filepath = pathlib.Path(sweep_config_file).resolve()

    if not sweep_config_filepath.is_file() and sweep_config_object is None:
        raise ValueError(
            'sweep_config_object must be given if sweep_config_file is not an existing file'
        )
    
    if sweep_config_object is None:
        sweep_cfg = SweepConfiguration.from_file(sweep_config_filepath)
    else:
        sweep_cfg = deepcopy(sweep_config_object)
        sweep_config_filepath.parent.mkdir(parents=True, exist_ok=True)
        sweep_cfg.write(sweep_config_filepath)
    
    # Change working directory to the directory where the sweep config file is located
    cwd = os.getcwd()
    os.chdir(sweep_config_filepath.parent)

    # Get the simulations and push them onto a SimulationPool to run them
    simulations = _create_simulations(sweep_cfg, random_seed)
    pool = SimulationPool(thread_count)

    if echo_status: _preamble(sweep_cfg, thread_count)

    for simulation in simulations:
        pool.push(simulation)
    
    if echo_status: _report_pool_status(pool, len(simulations), polling_interval)
    
    # Wait for all simulations to finish
    pool.wait()

    sweep_result = _get_sweep_result(sweep_cfg)

    # Restore working directory
    os.chdir(cwd)

    if echo_status: _postamble(sweep_result)

    return sweep_result


def simulation_dirs(sweep_cfg: SweepConfiguration) -> list[pathlib.Path]:
    """
    Returns a list of all simulation (sub)directories that will be used in this sweep.  Useful for
    finding the files afterward.
    """
    simulation_dirs = []

    for temperature, density in _sweep_range(sweep_cfg):
        simulation_dirs.append(_get_simulation_dir(sweep_cfg, temperature, density))
    
    return simulation_dirs


def _preamble(sweep_cfg: SweepConfiguration, thread_count: int):
    """
    Prints information about the sweep before running it
    """
    preamble = """\
        ================= Lennard-Jones Sweep Simulation ==================

        Temperature: {temp_steps} points in the range [{temp_start}, {temp_stop}]
        Density: {d_steps} points in the range [{d_start}, {d_stop}]
        
        Number of Particles: {particle_count}
        Time Step: {time_step}

        Running {job_count} jobs over {thread_count} threads

        Begin simulation sweep...""".format(
            temp_start=sweep_cfg.system.temperature_start,
            temp_stop=sweep_cfg.system.temperature_stop,
            temp_steps=sweep_cfg.system.temperature_steps,
            d_start=sweep_cfg.system.density_start,
            d_stop=sweep_cfg.system.density_stop,
            d_steps=sweep_cfg.system.density_steps,
            particle_count=sweep_cfg.system.particle_count,
            time_step=sweep_cfg.system.time_delta,
            job_count=sweep_cfg.system.temperature_steps * sweep_cfg.system.density_steps,
            thread_count=thread_count
        )

    print(textwrap.dedent(preamble), flush=True)


def _postamble(result: SweepResult):
    """
    Indicate the status after the sweep is finished
    """
    postamble = f"""\
        End simulation sweep
        
        Job status:
        Completed: {len(result.completed)}
        Aborted during Equilibration: {len(result.equilibration_aborted)}
        Aborted during Observation: {len(result.observation_aborted)}"""
    
    print(textwrap.dedent(postamble), flush=True)


def _get_sweep_result(sweep_cfg: SweepConfiguration) -> SweepResult:
    """
    Accumulates the data from the event logs of all of the simulations in the sweep into a
    SweepResult object.
    """
    sweep_result = SweepResult()

    for simulation_dir in simulation_dirs(sweep_cfg):
        run_config_file = simulation_dir / sweep_cfg.templates.run_config_file

        result = get_simulation_result(run_config_file)

        if result == SimulationResult.completed:
            category = sweep_result.completed
        elif result == SimulationResult.equilibration_aborted:
            category = sweep_result.equilibration_aborted
        elif result == SimulationResult.observation_aborted:
            category = sweep_result.observation_aborted
        
        category.append(simulation_dir)
    
    return sweep_result


def _report_pool_status(pool: SimulationPool, job_count: int, polling_interval: float = 0.5):
    """
    Polls the given SimulationPool and reports its status until all jobs finish.
    """
    while True:
        status = pool.status()
        print(
            'Jobs queued: {}, Running: {}, Completed: {}             '.format(
                status.waiting, status.running, status.completed
            ),
            flush=True,
            end='\r'
        )
        if status.completed == job_count:
            break
        time.sleep(polling_interval)
    
    # We need one final newline so that the next print statement will not overwrite the above
    # output
    print()


def _create_simulations(
    sweep_cfg: SweepConfiguration,
    random_seed: Union[None, int, FunctionType, BuiltinFunctionType] = None,
) -> list[Simulation]:
    """
    Creates a list of Simulation objects for each (temperature, density) pair in the sweep.

    Precondition: Our working directory is the same directory that contains the sweep config file,
        so that relative directories from this location make sense.
    
    Postcondition: All of the individual simulation directories will be created, and the individual
        run config files will be written (which allows simulations to be easily re-run)
    """
    simulations = []

    for temperature, density in _sweep_range(sweep_cfg):
        # Get the directory where the individual simulation will be run
        simulation_dir = _get_simulation_dir(sweep_cfg, temperature, density)
        run_config_file = simulation_dir / sweep_cfg.templates.run_config_file

        # Create run configuration object (introduces default random seed)
        run_cfg = _create_run_configuration(sweep_cfg, temperature, density)

        # Determine whether random seed should be updated
        if random_seed is None and run_config_file.is_file():
            existing_cfg = Configuration.from_file(run_config_file)
            run_cfg.system.random_seed = existing_cfg.system.random_seed
        elif isinstance(random_seed, int):
            run_cfg.system.random_seed = random_seed
        elif isinstance(random_seed, (FunctionType, BuiltinFunctionType)):
            run_cfg.system.random_seed = random_seed()
        
        # Write config to file (possibly overwrites with new sweep_cfg data)
        run_config_file.parent.mkdir(parents=True, exist_ok=True)
        run_cfg.write(run_config_file)

        # We cannot change working directory for each individual simulation, so before creating
        # the Simulation object, we must prepend the simulation_dir to the output filepaths
        _prepend_simulation_dir(simulation_dir, run_cfg)

        # Now create a Simulation and append it to the list
        simulations.append(Simulation(run_cfg))
    
    return simulations


def _sweep_range(sweep_cfg: SweepConfiguration) -> Iterator[tuple[float, float]]:
    """
    Returns an iterator over (temperature, density) pairs for each point in the sweep.
    """
    # np.array.tolist() not only creates a Python list, but also converts Numpy dtypes to Python
    # builtin types.  This is necessary as assumptions are made elsewhere about the type of
    # the temperature, density being Python's `float`.

    temperatures = np.linspace(
        sweep_cfg.system.temperature_start,
        sweep_cfg.system.temperature_stop,
        sweep_cfg.system.temperature_steps,
        endpoint=True
    ).tolist()

    densities = np.linspace(
        sweep_cfg.system.density_start,
        sweep_cfg.system.density_stop,
        sweep_cfg.system.density_steps,
        endpoint=True
    ).tolist()

    return itertools.product(temperatures, densities)


def _get_simulation_dir(
    sweep_cfg: SweepConfiguration,
    temperature: float,
    density: float
) -> pathlib.Path:
    """
    Returns the simulation directory for a corresponding temperature and density.
    """
    return pathlib.Path(sweep_cfg.templates.directory.format(
        temperature=temperature, density=density
    ))


def _prepend_simulation_dir(simulation_dir: pathlib.Path, run_cfg: Configuration):
    """
    Modifies run_cfg to prepend simulation_dir onto the paths.
    """
    run_cfg.filepaths.event_log = str(simulation_dir / run_cfg.filepaths.event_log)
    run_cfg.filepaths.thermodynamic_log = str(simulation_dir / run_cfg.filepaths.thermodynamic_log)
    run_cfg.filepaths.observation_log = str(simulation_dir / run_cfg.filepaths.observation_log)
    run_cfg.filepaths.snapshot_log = str(simulation_dir / run_cfg.filepaths.snapshot_log)


def _create_run_configuration(
    sweep_cfg: SweepConfiguration,
    temperature: float,
    density: float
) -> Configuration:
    """
    Creates a run configuration from the sweep configuration and a given temperature and density.
    """
    run_cfg = Configuration()

    run_cfg.system.temperature = temperature
    run_cfg.system.density = density
    run_cfg.system.particle_count = sweep_cfg.system.particle_count
    run_cfg.system.cutoff_distance = sweep_cfg.system.cutoff_distance
    run_cfg.system.time_delta = sweep_cfg.system.time_delta

    run_cfg.equilibration.name = (sweep_cfg.templates.phase_name.format(
        temperature=temperature, density=density, name=sweep_cfg.equilibration.name
    ))
    run_cfg.equilibration.tolerance = sweep_cfg.equilibration.tolerance
    run_cfg.equilibration.sample_size = sweep_cfg.equilibration.sample_size
    run_cfg.equilibration.adjustment_interval = sweep_cfg.equilibration.adjustment_interval
    run_cfg.equilibration.steady_state_time = sweep_cfg.equilibration.steady_state_time
    run_cfg.equilibration.timeout = sweep_cfg.equilibration.timeout

    run_cfg.observation.name = (sweep_cfg.templates.phase_name.format(
        temperature=temperature, density=density, name=sweep_cfg.observation.name
    ))
    run_cfg.observation.tolerance = sweep_cfg.observation.tolerance
    run_cfg.observation.sample_size = sweep_cfg.observation.sample_size
    run_cfg.observation.observation_interval = sweep_cfg.observation.observation_interval
    run_cfg.observation.observation_count = sweep_cfg.observation.observation_count

    run_cfg.filepaths.event_log = sweep_cfg.filenames.event_log
    run_cfg.filepaths.thermodynamic_log = sweep_cfg.filenames.thermodynamic_log
    run_cfg.filepaths.observation_log = sweep_cfg.filenames.observation_log
    run_cfg.filepaths.snapshot_log = sweep_cfg.filenames.snapshot_log

    return run_cfg
