#!/usr/bin/env python3.11

import math
import random
import argparse
import pathlib
import itertools
import json
import bounds as bd

#compute running time under amdahl_law
#exect_time: total execution time
#nproc: number of porcessor assigned to the task 
#amdhal_p: percent of parallelizable work 
#return: the runtime (rounded up)
def amdahl_law(exec_time, nproc, amdahl_p):
    h = exec_time * ((1-amdahl_p) + amdahl_p / nproc)
    h_ceil = math.ceil(h)
    return h_ceil

class Task:
    #even numbers up to the upper bound
    def evens(ub):
        return [2 * i for i in range(1, ub // 2 + 1)]

    def __init__(self, id, wmax, rmax): 
        self.id = id
        self.rmax = rmax #maximum number of repetitions (randomly generated) 
        self.wmax = wmax #maximum number of cpus to assign to the task. 
        self.repeat = random.randint(1, rmax)
        self.effort = random.randint(20, 100) #total work to be done (area of the task).
        self.configs = []

    # generate the possible configurations for the task
    def gen_configs(self, W, amdahl_p):
        nconfigs = round(self.wmax / 2)
        ws = list(random.sample(range(1, min(W, self.wmax)), nconfigs))
        ws.sort()

        #add always the first one
        w = ws[0]
        h = amdahl_law(self.effort, w, amdahl_p)
        self.configs.append((w, h))
        for w in ws[1:]:
            h = amdahl_law(self.effort, w, amdahl_p)
            if h > 0 and self.configs[-1][1] != h:
                self.configs.append((w, h))

        if not self.configs:
            self.configs.append((1, self.effort))


class Instance:
    def __init__(self, W, ntasks, wmax, rmax, amdahl_p, seed):
        self.W = W #strip width
        self.ntasks = ntasks #number of tasks to schedule
        self.wmax = wmax #maximum number of cpus assignable to a task 
        self.rmax = rmax #maximum number of repetitions for each task
        self.amdahl_p = amdahl_p #percent of parallelizable work per task.
        self.seed = seed

        self.name = f'amdahl{int(self.amdahl_p*100)}{self.ntasks:02}{self.seed:01}'

        self.tasks = [Task(id, self.wmax, self.rmax) for id in range(self.ntasks)]
        for task in self.tasks:
            task.gen_configs(W, self.amdahl_p) 

        assert all([len(t.configs) > 0 for t in self.tasks])

        self.nitems = sum(len(task.configs) for task in self.tasks)
        self.reff = sum(task.repeat for task in self.tasks)

        #bounds
        items = list(itertools.chain.from_iterable([(task.configs[0][0], task.configs[0][1])] * task.repeat
                                                   for task in self.tasks))
        self.lb = -1
        self.ub = -1 

    def statistics(self):
        nshelves = sum(len(task.configs) * task.repeat for task in self.tasks)
        print(f'{self.name}_{self.rmax:02}_{self.wmax:02} nshelves=', nshelves)

    def write_json(self, dir_path):
        d = {'instance': 
                {'name': self.name, 
                'rmax': f'{self.rmax:02}',
                'wmax': f'{self.wmax:02}',
                'W': self.W,
                'seed': self.seed,
                'ntasks': self.ntasks,
                'nitems': self.nitems,
                'reff': self.reff,
                'lb': self.lb,
                'ub': self.ub,
                'tasks': [{'id': task.id,
                            'effort': task.effort,
                            'repeat': task.repeat,
                            'configs': [{'width': c[0], 'height': c[1]} for c in task.configs]
                            } for task in self.tasks]
            }
        }

        file_name = '_'.join([self.name, f'{self.rmax:02}', f'{self.wmax:02}'])
        file_path = (dir_path / file_name).with_suffix('.json')
        
        with open(file_path, 'w') as f:
            json.dump(d, f, indent=4)


#generate the instnces. parametes are lists of possible values for the 
#instance parameters.
def gen_instances(dir_path, seeds, strip_widths, number_of_tasks, maximum_repetitions, maximum_processors, amdahl_percentuals):
    dir_path.mkdir(parents=True, exist_ok=True)
    for W, ntasks, rmax, wmax, amdahl_p, seed in itertools.product(strip_widths, number_of_tasks, maximum_repetitions,
                                                             maximum_processors, amdahl_percentuals, seeds): 

        output_dir = dir_path / f'seed{seed}'
        output_dir.mkdir(parents=True, exist_ok=True)
        inst = Instance(W, ntasks, wmax, rmax, amdahl_p, seed)
        # inst.statistics()
        inst.write_json(output_dir)


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('output_dir', type=pathlib.Path, help='output dir')
    
    args = parser.parse_args()
    
    print('*****************class1*****************\n')
    gen_instances(args.output_dir, seeds=[1,2,3,4,5], strip_widths=[16], number_of_tasks=[10,15,20,30,50], maximum_repetitions=[3,5], maximum_processors=[8,16], amdahl_percentuals=[0.8, 0.95])