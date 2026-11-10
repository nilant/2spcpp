# Two-Dimensional Packing for Static Task Scheduling in High-Performance Computing

Code and benchmark instances for the **2SPCluster** problem: scheduling
*malleable* tasks on a cluster of `W` identical CPUs as a two-dimensional strip
packing problem.

## Problem

- The strip has width `W` (the number of CPUs) and unbounded height (time).
- Each task must be executed `repeat` times.
- Every execution can use a different number of CPUs. Each choice is a
  *configuration* `(width, height)`, where `width` is the number of CPUs and
  `height` is the running time. Running times follow Amdahl's law:
  `height = ceil(effort * ((1 - p) + p / width))`, with `p` the parallelizable
  fraction.
- A schedule places one rectangle for every execution of every task, without
  overlaps. The objective is to **minimize the height** of the strip (the
  makespan).

## Repository layout

| Path | Content |
| --- | --- |
| `gen_instances.py` | Random instance generator |
| `data.zip` | Benchmark instances (unzips to `data/`, 1440 JSON files) |
| `src/` | C++20 solvers (see below) |
| `libs/` | Vendored single-header libraries: `json.hpp` (nlohmann), `argh.h`, `BS_thread_pool.hpp` |
| `cmake/FindGUROBI.cmake` | CMake module that locates Gurobi |
| `Dockerfile`, `build.sh` | Reproducible build inside Docker |
| `build_local.sh` | Build with a local Gurobi installation |
| `requirements.txt` | Python dependencies (analysis and plotting) |

## Algorithms

Every algorithm is a separate executable built from `src/`:

| Executable | Description |
| --- | --- |
| `bottom_left` | Bottom-left heuristic. Each task uses its first (narrowest) configuration. Also reports an area lower bound. |
| `bottom_left_plus` | Improved bottom-left. It places items at the leftmost or rightmost feasible position, whichever lines up better with the neighbouring item. |
| `shelves` | MIP that packs the items in shelves (items sorted by height). Gives good solutions quickly. |
| `coord` | Coordinate MIP (Gurobi). The binary variable `x[i,p,q]` is 1 if item `i` has its bottom-left corner at `(p, q)`. Its grid height is bounded by the `bottom_left` solution. |
| `coord2` | Variant of `coord` in which every repetition of a task is a separate task. It is bounded with `bottom_left_plus`. Built by CMake but not copied to `bin/` by `build.sh`. |
| `matheuristic` | Solves `shelves` for 10 s, then repeatedly merges pairs of shelves (pairs chosen by a matching MIP, each merged pair re-optimized with `coord`) until one block remains. |

## Building

Gurobi (10.x) and a valid licence are required for the MIP-based programs.
`fmt` and CMake >= 3.22 are also needed.

**With Docker** (downloads Gurobi and compiles in the image, then copies the
binaries to `bin/`):

```bash
./build.sh
```

**Locally** (expects Gurobi in `/opt/gurobi/linux64`, or set `GUROBI_HOME`):

```bash
./build_local.sh
```

Executables end up in `bin/`.

## Instances

### Generating

```bash
./gen_instances.py data/
```

This writes one JSON file per instance, named with a random UUID (the
readable name is inside the file). Running it again adds new files and never
overwrites existing ones. The default parameters (edit the bottom of the
script to change them) are:

- `W = 16`
- 5, 10, 15, 20, 25 or 30 tasks
- `rmax` in {3, 5}: each task is repeated between 1 and `rmax` times
- `wmax` in {8, 16}: largest number of CPUs a task can use
- parallel fraction 99%
- 5 seeds

That makes 6 x 2 x 2 x 5 = 120 instances per run. Note that `seed` is stored in
the instance name but is **not** used to seed the random generator, so runs are
not reproducible.

The benchmark set used in the experiments is in `data.zip`:

```bash
unzip data.zip   # creates data/
```

### File format

```json
{
  "instance": {
    "name": "amdahl990501",
    "alpha": 99.0,       // parallelizable percentage
    "rmax": 3,           // max repetitions of a task
    "wmax": 8,           // max CPUs a task can use
    "W": 16,             // strip width
    "seed": 1,
    "ntasks": 5,
    "nitems": 17,        // total number of configurations
    "reff": 9,           // total repetitions over all tasks
    "lb": 41.6,          // area lower bound
    "ub": -1,
    "tasks": [
      {
        "id": 0,
        "effort": 63,    // sequential work
        "repeat": 2,
        "configs": [ { "width": 1, "height": 63 }, { "width": 3, "height": 22 } ]
      }
    ]
  }
}
```

(The `//` comments above are only for explanation; the real files are plain
JSON.)

## Running

All executables take the instance file as first argument:

```bash
bin/matheuristic data/<instance>.json --timelimit 600 --threads 4
bin/shelves      data/<instance>.json
bin/bottom_left  data/<instance>.json
```

| Option | Default | Meaning |
| --- | --- | --- |
| `--timelimit` | 3600 | Time limit in seconds (Gurobi) |
| `--memlimit` | 16 | Soft memory limit in GB (Gurobi) |
| `--threads` | 1 | Number of Gurobi threads |
| `--seed` | 0 | Gurobi random seed |
| `--iterlimit` | 500 | Parsed but currently unused |

**The result is printed to stdout as a single JSON object on a line prefixed
with `[RESULT]`**, and the instance file is not modified. Before solving, each
program also prints a `##### Solving instance <name>...` line to stdout. For
example:

```
##### Solving instance my_instance...
[RESULT] {"obj":50.0,"bound":44.1,"gap":0.12,"runtime":1.2}
```

Heuristics (`bottom_left`, `bottom_left_plus`, `matheuristic`) print `obj`,
`bound` and `runtime`; the MIP programs (`shelves`, `coord`, `coord2`) print
`obj`, `bound`, `gap` and `runtime`.

- `obj` is the height of the best solution found.
- `bound` is a lower bound.
- `gap` is the relative MIP gap (MIP programs only).
- `obj`, `bound`, `gap` and `runtime` are `-1` if the solver found no solution.

Progress and error messages go to stderr. Stdout also contains the
`#####` line and the Gurobi log (which can be silenced with `OutputFlag`), so
to collect the results keep only the lines starting with `[RESULT]`, e.g.
`bin/shelves data/<instance>.json | grep '^\[RESULT\]'`. `coord` also writes the Gurobi solution file `<instance>_sol.json` next to the instance.

## Python tools

`requirements.txt` lists the dependencies (pandas, matplotlib, openpyxl, ...)
for analysing the results:

```bash
python -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
```
