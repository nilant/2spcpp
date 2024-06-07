#! /usr/bin/env python3

import json
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches

def display_rectangles(rectangles):
    fig, ax = plt.subplots()

    for (x, y, width, height) in rectangles:
        rect = patches.Rectangle((x, y), width, height, linewidth=1, edgecolor='k', facecolor='g')
        ax.add_patch(rect)
    
    # Set limits for the plot based on the rectangles
    all_x = [x for x, _, w, _ in rectangles] + [x + w for x, _, w, _ in rectangles]
    all_y = [y for _, y, _, h in rectangles] + [y + h for _, y, _, h in rectangles]
    ax.set_xlim(min(all_x), max(all_x))
    ax.set_ylim(min(all_y), max(all_y))
    # ax.set_xticks(np.arange(0, max(all_x)));
    # ax.set_yticks(np.arange(0, max(all_y)));

    plt.gca().set_aspect('equal', adjustable='box')
    plt.show()


if __name__ == "__main__":

    with open(sys.argv[1], 'r') as f:
        j = json.load(f)
    
    rects = [(item['x'], item['y'], item['w'], item['h']) for item in j['solution']]
    display_rectangles(rects)