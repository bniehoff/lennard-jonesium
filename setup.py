import sys
import setuptools

from skbuild import setup

# Require pytest-runner only when running tests
pytest_runner = (['pytest-runner>=2.0,<3dev']
                 if any(arg in sys.argv for arg in ('pytest', 'test'))
                 else [])

setup_requires = pytest_runner

setup(
    name="lennardjonesium",
    version="0.3.2",
    description="A simulator for the Lennard-Jones substance",
    author='Ben Niehoff',
    license="GPL v3.0",
    packages=[
        'lennardjonesium',
        'lennardjonesium.tools',
        'lennardjonesium.simulation',
        'lennardjonesium.orchestration',
        ],
    package_dir = {
        '': 'src/python',
        },
    scripts=['src/python/lennardjonesium/scripts/experiment.py'],
    install_requires=['cython'],
    tests_require=['pytest'],
    setup_requires=setup_requires,
    cmake_languages = ('CXX',)
)
