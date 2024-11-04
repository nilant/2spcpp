#! /usr/bin/env python3

import json
import pathlib
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches


if __name__ == "__main__":

    indir = pathlib.Path(sys.argv[1])

    for infile in indir.glob("*.json"):
        with open(infile, 'r') as f:
            j = json.load(f)

        algos = ['shelves', 'bl', 'coord']
        fig, ax = plt.subplots(1, 3)

        max_x = 0
        max_y = 0

        min_x = 100
        min_y = 100

        for i in range(len(algos)):
            algo = algos[i]
            obj = j[algo]['obj']
            rects = [(item['task_id'], item['x'], item['y'], item['w'], item['h'])
                     for item in j[algo]['items']]

            for (tid, x, y, width, height) in rects:
                rect = patches.Rectangle(
                    (x, y), width, height, linewidth=1, edgecolor='k', facecolor='g')
                ax[i].add_patch(rect)
                cx = x + width / 2.0
                cy = y + height / 2.0
                ax[i].annotate(tid, (cx, cy), color='w', fontsize=3, ha='center', va='center')

            all_x = [x for _, x, _, w, _ in rects] + \
                [x + w for _, x, _, w, _ in rects]
            all_y = [y for _, _, y, _, h in rects] + \
                [y + h for _, _, y, _, h in rects]

            min_x = min(min_x, min(all_x))
            max_x = max(max_x, max(all_x))

            min_y = min(min_y, min(all_y))
            max_y = max(max_y, max(all_y))

            ax[i].set_title(algo+': ' + str(obj))
            ax[i].set_xticks(np.arange(0, max_x, 1))
            # ax[i].set_yticks(np.arange(0, max_y, 1))
            ax[i].tick_params(axis='both', labelsize=5)

        for i in range(len(algos)):
            ax[i].set_xlim(min_x, max_x)
            ax[i].set_ylim(min_y, max_y)

        outfile = infile.with_suffix('.pdf')
        plt.savefig(outfile)
