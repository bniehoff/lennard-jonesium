"""
Test Configuration class
"""

import unittest
import pathlib
from debugpy import configure

from lennardjonesium.simulation import Configuration

class TestConfiguration(unittest.TestCase):
    def test_from_dict(self):
        dictionary = {
            'system': {
                'temperature': 0.7,
                'density': 0.3,
                'particle_count': 10
            },
            'equilibration': {
                'name': "T=0.7, d=0.3 Equilibration Phase",
                'timeout': 2000
            },
            'observation': {
                'name': "T=0.7, d=0.3 Observation Phase",
                'observation_count': 5
            },
            'filepaths': {
                'event_log': 'data/events.log'
            }
        }

        configuration = Configuration.from_dict(dictionary)

        self.assertEqual(10, configuration.system.particle_count)
        self.assertEqual(2000, configuration.equilibration.timeout)
    
    def test_read_write_files(self):
        test_dir = pathlib.Path('temp/test_configuration')
        test_dir.mkdir(parents=True, exist_ok=True)
        test_file = test_dir / 'test.ini'

        # Just use a default configuration
        source_conf = Configuration()

        source_conf.write(test_file)

        dest_conf = Configuration.from_file(test_file)

        self.assertEqual(source_conf, dest_conf)

        test_file.unlink()
        test_dir.rmdir()
