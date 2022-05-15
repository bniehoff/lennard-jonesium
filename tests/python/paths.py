"""
Some useful paths for tests that deal with reading and writing files.
"""


import pathlib


temp_dir = pathlib.Path('temp/')
python_dir = pathlib.Path('tests/python/')
lj_dir = python_dir / 'lennardjonesium'


def file_stem(filename: str):
    return pathlib.Path(filename).stem
