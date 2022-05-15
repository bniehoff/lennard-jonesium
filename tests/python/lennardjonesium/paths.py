"""
Some useful paths for tests that deal with reading and writing files.
"""


import pathlib


temp_dir = pathlib.Path('temp/')


def file_stem(filename: str):
    return pathlib.Path(filename).stem
