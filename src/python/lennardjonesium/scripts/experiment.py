#!/usr/bin/env python
"""
run_experiment.py

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

import argparse
import pathlib
import time

import lennardjonesium as lj

def parse_chunk_list(chunks: str) -> list[int]:
    output = []
    for entry in chunks.strip('[]').split(','):
        first, _, second = entry.partition('-')
        if second:
            output.extend(range(int(first), int(second) + 1))
        else:
            output.append(int(first))
    return output

def rest(rest_time: float, polling_interval: float):
    print()
    while rest_time > 0:
        print(f'Starting next chunk in {rest_time} seconds     ', end='\r')
        rest_time -= polling_interval
        time.sleep(polling_interval)
    print('\n')


parser = argparse.ArgumentParser('Run an experiment from a sweep configuration .ini file')

parser.add_argument('sweep_config_file', type=pathlib.Path)
parser.add_argument('--poll-interval', type=float, default=1.0)
parser.add_argument('--threads', type=int, default=4)
parser.add_argument('--chunk-count', type=int, default=1)
parser.add_argument('--chunk-indices', default='all')
parser.add_argument('--rest-time', type=float, default=5.0)

args = parser.parse_args()

if args.chunk_indices == 'all':
    chunk_indices = range(args.chunk_count)
else:
    chunk_indices = parse_chunk_list(args.chunk_indices)

first_chunk = True
for chunk_index in chunk_indices:
    if not first_chunk:
        rest(args.rest_time, args.poll_interval)
    first_chunk = False

    result = lj.run_sweep(
        sweep_config_file=args.sweep_config_file,
        echo_status=True,
        polling_interval=args.poll_interval,
        thread_count=args.threads,
        random_seed=lj.SeedGenerator().get,
        chunk_count=args.chunk_count,
        chunk_index=chunk_index
    )

    # List the failed simulations so they can be rerun individually if desired
    if result.equilibration_aborted:
        print()
        print('The following simulations failed to equilibrate:')
        for entry in result.equilibration_aborted:
            print(entry.simulation_dir)
    
    if result.observation_aborted:
        print()
        print('The following simulations failed to collect all observations:')
        for entry in result.observation_aborted:
            print('{directory} Observations: {observations}'.format(
                directory=entry.simulation_dir,
                observations=len(entry.event_data.observations_recorded)
            ))
