#! /usr/bin/env python3

import pathlib
import argparse
import tomllib
import json


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('input_dir', type=pathlib.Path, help='directory of solutions files')
    parser.add_argument('--config', type=pathlib.Path, help='configuration file')

    args = parser.parse_args()

    with open(args.config, 'rb') as f:
        config = tomllib.load(f)

    resnames = [pathlib.Path(exec).stem for exec in config['models']]
    for resname in resnames:
        for input in args.input_dir.glob('**/*.json'):

            with open(input, 'r') as f:
                j = json.load(f)

            if j[resname]['obj'] == -1:
                j[resname]['obj'] = float('nan')
            if j[resname]['bound'] == -1:
                j[resname]['bound'] = float('nan')
            if j[resname]['runtime'] == -1 or j[resname]['runtime'] > config['timelimit']:
                j[resname]['runtime'] = float('nan')
            j[resname]['gap'] = abs(j[resname]['obj'] - j[resname]['bound']) / abs(j[resname]['obj'])
            
            with open(input, 'w') as f:
                json.dump(j, f, indent=4)