#!/usr/bin/env python3

# Random instance generator for the 2SPCluster problem (strip packing with
# malleable tasks). Each task can run on several numbers of CPUs (its
# "configurations"); a configuration is a rectangle of width = #CPUs and
# height = running time. Usage: ./gen_instances.py <output_dir>
import argparse
import itertools
import json
import math
import pathlib
import random
import uuid


# Amdahl's law: running time of a task when executed on `nproc` processors.
#   exec_time: sequential execution time (the task "effort")
#   nproc:     number of processors assigned to the task
#   amdahl_p:  fraction of the work that is parallelizable (0..1)
# Returns the running time rounded up.
def amdahl_law(exec_time, nproc, amdahl_p):
    h = exec_time * ((1 - amdahl_p) + amdahl_p / nproc)
    h_ceil = math.ceil(h)
    return h_ceil


# A task to schedule: it must be executed `repeat` times, and each execution
# can pick one of several (width, height) configurations.
class Task:
    # Even numbers in [2, ub] (helper, currently unused).
    def evens(self, ub):
        return [2 * i for i in range(1, ub // 2 + 1)]

    def __init__(self, id, wmax, rmax):
        self.id = id
        self.rmax = rmax  # maximum number of repetitions (randomly generated)
        self.wmax = wmax  # maximum number of cpus to assign to the task.
        self.repeat = random.randint(1, rmax)
        self.effort = random.randint(
            20, 100
        )  # total work to be done (area of the task).
        self.configs = []

    # Generate the configurations of the task: pick wmax/2 distinct random widths
    # (in [1, min(W, wmax))) and compute the matching height with Amdahl's law.
    # Widths whose height is equal to the previous kept one are discarded (more
    # CPUs but no speed-up), so the heights are strictly decreasing in the width.
    def gen_configs(self, W, amdahl_p):
        nconfigs = round(self.wmax / 2)
        ws = list(random.sample(range(1, min(W, self.wmax)), nconfigs))
        ws.sort()

        # add always the first one
        w = ws[0]
        h = amdahl_law(self.effort, w, amdahl_p)
        self.configs.append((w, h))
        for w in ws[1:]:
            h = amdahl_law(self.effort, w, amdahl_p)
            if h > 0 and self.configs[-1][1] != h:
                self.configs.append((w, h))

        if not self.configs:
            self.configs.append((1, self.effort))


# A full problem instance. Note: `seed` is only a label used in the instance
# name and in the JSON, it is NOT passed to random.seed(), so runs are not
# reproducible.
class Instance:
    def __init__(self, W, ntasks, wmax, rmax, amdahl_p, seed):
        self.W = W  # strip width
        self.ntasks = ntasks  # number of tasks to schedule
        self.wmax = wmax  # maximum number of cpus assignable to a task
        self.rmax = rmax  # maximum number of repetitions for each task
        self.amdahl_p = amdahl_p  # percent of parallelizable work per task.
        self.seed = seed

        # name encodes: parallel percentage, number of tasks and seed
        self.name = f"amdahl{int(self.amdahl_p * 100)}{self.ntasks:02}{self.seed:01}"

        self.tasks = [Task(id, self.wmax, self.rmax) for id in range(self.ntasks)]
        for task in self.tasks:
            task.gen_configs(W, self.amdahl_p)

        assert all(len(t.configs) > 0 for t in self.tasks)

        self.nitems = sum(len(task.configs) for task in self.tasks)
        self.reff = sum(task.repeat for task in self.tasks)

        # bounds
        items = list(
            itertools.chain.from_iterable(
                [(task.configs[0][0], task.configs[0][1])] * task.repeat
                for task in self.tasks
            )
        )
        # Lower bound on the height: area of the items (first, i.e. narrowest,
        # configuration of each task, repeated `repeat` times) divided by W.
        self.lb = sum(item[0] * item[1] for item in items) / self.W
        self.ub = -1

    # Print the number of shelves (items x repetitions) of the instance.
    def statistics(self):
        nshelves = sum(len(task.configs) * task.repeat for task in self.tasks)
        print(f"{self.name}_{self.rmax:02}_{self.wmax:02} nshelves=", nshelves)

    # Serialize the instance as JSON. The file is named with a random UUID so that
    # re-running the generator never overwrites previous instances; the readable
    # name is stored inside the file ("name" field).
    def write_json(self, dir_path):
        d = {
            "instance": {
                "name": self.name,
                "alpha": self.amdahl_p * 100,
                "rmax": self.rmax,
                "wmax": self.wmax,
                "W": self.W,
                "seed": self.seed,
                "ntasks": self.ntasks,
                "nitems": self.nitems,
                "reff": self.reff,
                "lb": self.lb,
                "ub": self.ub,
                "tasks": [
                    {
                        "id": task.id,
                        "effort": task.effort,
                        "repeat": task.repeat,
                        "configs": [
                            {"width": c[0], "height": c[1]} for c in task.configs
                        ],
                    }
                    for task in self.tasks
                ],
            }
        }

        file_name = str(uuid.uuid4())
        file_path = (dir_path / file_name).with_suffix(".json")

        with open(file_path, "w") as f:
            json.dump(d, f, indent=4)


# Generate one instance for every combination of the given parameters (each
# parameter is a list of possible values) and write them as JSON in dir_path.
def gen_instances(
    dir_path,
    seeds,
    strip_widths,
    number_of_tasks,
    maximum_repetitions,
    maximum_processors,
    amdahl_percentuals,
):
    dir_path.mkdir(parents=True, exist_ok=True)
    for W, ntasks, rmax, wmax, amdahl_p, seed in itertools.product(
        strip_widths,
        number_of_tasks,
        maximum_repetitions,
        maximum_processors,
        amdahl_percentuals,
        seeds,
    ):
        dir_path.mkdir(parents=True, exist_ok=True)
        inst = Instance(W, ntasks, wmax, rmax, amdahl_p, seed)
        # inst.statistics()
        inst.write_json(dir_path)


# Default benchmark set ("class1"): W=16, 5..30 tasks, rmax in {3,5},
# wmax in {8,16}, alpha=99%, 5 seeds => 6*2*2*5 = 120 instances.
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("output_dir", type=pathlib.Path, help="output dir")

    args = parser.parse_args()

    print("*****************class1*****************\n")
    gen_instances(
        args.output_dir,
        seeds=[1, 2, 3, 4, 5],
        strip_widths=[16],
        number_of_tasks=[5, 10, 15, 20, 25, 30],
        maximum_repetitions=[3, 5],
        maximum_processors=[8, 16],
        amdahl_percentuals=[0.99],
    )
