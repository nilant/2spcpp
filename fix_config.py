#!/usr/bin/env python3.11

import math
import json
import argparse
import pathlib

def amdahl_law(exec_time, nproc, amdahl_p):
    h = exec_time * ((1-amdahl_p) + amdahl_p / nproc)
    h_ceil = math.ceil(h)
    return h_ceil

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input_dir', type=pathlib.Path, help='instances directory')

    args = parser.parse_args();

    for input in args.input_dir.glob('**/*.json'):
        with open(input, 'r') as f:
            j = json.load(f)

            alpha = j['instance']['alpha'] / 100
            for task in j['instance']['tasks']:
                effort = task['effort']
                for config in task['configs']:
                    width = config['width']
                    config['height'] = amdahl_law(effort, width, alpha)


        with open(input, 'w') as f:
            json.dump(j, f, indent=4)