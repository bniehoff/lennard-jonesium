import sys

from skbuild import setup

# Require pytest-runner only when running tests
pytest_runner = (['pytest-runner>=2.0,<3dev']
                 if any(arg in sys.argv for arg in ('pytest', 'test'))
                 else [])

setup_requires = pytest_runner

setup(
    name="lennardjones",
    version="0.0.1",
    description="A simulator for Lennard-Jones particles",
    author='Ben Niehoff',
    license="GPL v3.0",
    packages=['lennardjones'],
    package_dir = {'': 'src'},
    install_requires=['cython'],
    tests_require=['pytest'],
    setup_requires=setup_requires
)
