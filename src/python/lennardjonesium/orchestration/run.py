"""
run.py

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


import os
import pathlib
from typing import Union, Optional
from types import FunctionType, BuiltinFunctionType
from copy import deepcopy
from inspect import cleandoc

from lennardjonesium.simulation import Configuration, Simulation

def run(
    config_file: Union[str, pathlib.Path],
    echo_status: bool = True,
    random_seed: Union[None, int, FunctionType, BuiltinFunctionType] = None,
    config_object: Optional[Configuration] = None
):
    """
    Wrapper function for running a single simulation.

    :param config_file: File path to the config file which describes the simulation to be run.
        The output config file will be written to the same place (possibly after changing the
        random seed).  If this file does not already exist, then the config_object parameter MUST
        be given, and a config file will be created at this path.  If the config file does exist,
        it will be overwritten, perhaps with a fresh random seed.
    
    :param random_seed: Can be None, an int, or a function which takes 0 arguments.
        If None, then use the random seed from the config file.
        If an int, then use this random seed.
        If a function, then call this function to obtain a random seed.
    
    :param config_object: An optional Configuration object.  If this is given, then it will be used
        in preference over the config_file, and the config_file will be overwritten with the data
        provided by the config_object.  If a new random seed has been provided, then it will
        override the one given in the config_object.
    
    We will always output a config file with the random seed actually used, which will overwrite
    the original config file.

    TODO: Add some preamble that pretty-prints the Configuration data before the run, and
        congratulates the user when the run is finished.
    """

    config_filepath = pathlib.Path(config_file).resolve()

    if not config_filepath.is_file() and config_object is None:
        raise ValueError('config_object must be given if config_file is not an existing file')
    
    if config_object is None:
        cfg = Configuration.from_file(config_filepath)
    else:
        cfg = deepcopy(config_object)
    
    if isinstance(random_seed, int):
        cfg.system.random_seed = random_seed
    elif isinstance(random_seed, (FunctionType, BuiltinFunctionType)):
        cfg.system.random_seed = random_seed()
    
    # Write (or overwrite) the config file with the new random seed
    config_filepath.parent.mkdir(parents=True, exist_ok=True)
    cfg.write(config_filepath)

    if echo_status: _preamble(cfg)
    
    # Change working directory to the directory where the config file is located
    cwd = os.getcwd()
    os.chdir(config_filepath.parent)

    # Create the Simulation object and run the simulation
    sim = Simulation(cfg)
    sim.run(echo=echo_status)

    # Restore the working directory
    os.chdir(cwd)

    if echo_status: _postamble()


def _preamble(cfg: Configuration):
    """
    Prints information about the simulation to be run from the given Configuration object
    """

    print(
        cleandoc(f"""
            ==================== Lennard-Jones Simulation ====================
            
            Temperature: {cfg.system.temperature}
            Density: {cfg.system.density}
            Number of Particles: {cfg.system.particle_count}
            Time Step: {cfg.system.time_delta}
            Random Seed: {cfg.system.random_seed}

            Simulation will run in 2 phases:
            Phase 1: {cfg.equilibration.name}
            Phase 2: {cfg.observation.name}

            Results to be output to the following files:
            Events: {cfg.filepaths.event_log}
            Thermodynamics: {cfg.filepaths.thermodynamic_log}
            Observations: {cfg.filepaths.observation_log}
            Snapshots: {cfg.filepaths.snapshot_log}

            Begin Simulation...
            """
        ),
        flush=True
    )

def _postamble():
    """
    Prints information after simulation is finished
    """

    print("End Simulation", flush=True)
