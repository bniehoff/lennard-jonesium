"""
Test running a Simulation from a config file
"""

import unittest
import pathlib
import shutil

from lennardjonesium.simulation import Configuration, Simulation

from tests.python.paths import temp_dir


class TestSimulation(unittest.TestCase):
    def test_simulation_from_file(self):
        this_file = pathlib.Path(__file__)
        test_dir = temp_dir / this_file.stem
        input_file = this_file.parent / 'test_simulation.ini'

        cfg = Configuration.from_file(input_file)

        # Re-root directories onto test dir
        event_log = test_dir / cfg.filepaths.event_log
        thermodynamic_log = test_dir / cfg.filepaths.thermodynamic_log
        observation_log = test_dir / cfg.filepaths.observation_log
        snapshot_log = test_dir / cfg.filepaths.snapshot_log

        cfg.filepaths.event_log = str(event_log)
        cfg.filepaths.thermodynamic_log = str(thermodynamic_log)
        cfg.filepaths.observation_log = str(observation_log)
        cfg.filepaths.snapshot_log = str(snapshot_log)

        # Make sure all directories exist
        event_log.parent.mkdir(parents=True, exist_ok=True)
        thermodynamic_log.parent.mkdir(parents=True, exist_ok=True)
        observation_log.parent.mkdir(parents=True, exist_ok=True)
        snapshot_log.parent.mkdir(parents=True, exist_ok=True)

        # Create and run the simulation
        sim = Simulation(cfg)
        sim.run(echo=False)

        # Make sure observation log has the correct number of lines
        line_count = sum(1 for _ in open(observation_log))
        self.assertEqual(cfg.observation.observation_count + 1, line_count)

        # Clean up
        shutil.rmtree(test_dir)
