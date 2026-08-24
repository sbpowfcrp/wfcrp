# An Iterated Local Search Heuristic for the Offshore Wind Farm Cable Routing Problem

This repository contains the C++ implementation of metaheuristic algorithms for the Wind Farm Cable Routing Problem (WFCRP). It includes the proposed Iterated Local Search (ILS) and three baseline algorithms: Variable Neighborhood Search (VNS), Tabu Search (TS), and Simulated Annealing (SA).

## 📄 Abstract

The Wind Farm Cable Routing Problem (WFCRP) is an NP-hard optimization problem that minimizes the cost of inter-array cable infrastructure in offshore wind farms. Several metaheuristics, such as Tabu Search (TS), Simulated Annealing (SA), and Variable Neighborhood Search (VNS), have been proposed for this problem. However, these approaches often struggle to find high-quality solutions for instances with more than 100 turbines. This paper proposes an Iterated Local Search (ILS) algorithm for the WFCRP, a metaheuristic not previously explored for this problem. Computational experiments compare the proposed ILS against VNS, TS, and SA on 220 benchmark instances with up to 120 turbines. The results show that ILS produces the best solutions. Statistical analysis, using the Friedman and Nemenyi tests, confirms this superiority, showing that the ILS yields statistically better results across all instance sizes.

## 📂 Repository Structure

```
├── src/
│   ├── metaheuristics/
│   │   ├── base.cpp / base.hpp        # Abstract base class: run loop, time control, and result reporting
│   │   ├── ils.cpp  / ils.hpp         # Proposed ILS: sweep pool initialization, localized perturbation, best-improvement local search, and pool-based restart
│   │   ├── vns.cpp  / vns.hpp         # Variable Neighborhood Search baseline
│   │   ├── ts.cpp   / ts.hpp          # Tabu Search baseline (arc-based tabu list, variable tenure)
│   │   └── sa.cpp   / sa.hpp          # Simulated Annealing baseline (geometric cooling, MST-normalized acceptance)
│   ├── instance.cpp / instance.hpp    # Instance data: node coordinates, cable types (capacity & cost/m), substation capacity
│   ├── instance_handler.cpp / instance_handler.hpp  # Pre-computed edge lengths and helper queries
│   ├── solution.cpp / solution.hpp    # Solution representation: parent-array tree, penalized cost (cable cost + P1 + P2 + P3), 1-opt move and best-improvement local search
│   ├── sweep.cpp / sweep.hpp          # Sweep construction heuristic: radial partitioning + MST per group
│   ├── mst.cpp / mst.hpp              # Minimum Spanning Tree utility (used by Sweep and SA normalization)
│   ├── vector2d.hpp                   # 2-D vector arithmetic
│   ├── utils.hpp                      # Random number helpers (uniform sampling, shuffle)
│   └── main.cpp                       # CLI entry point: argument parsing and algorithm dispatch
├── CMakeLists.txt                     # CMake build configuration
└── build.sh                           # Helper build script (Release / Debug / Clean modes)
```

## 🔨 Build Instructions

### Using the helper script

```bash
chmod u+x build.sh
./build.sh          # Release mode (optimized)
./build.sh debug    # Debug mode (with symbols)
./build.sh clean    # Clean and rebuild
```

The executable is generated at `out/bin/wfcrp`.

### Manual CMake build

```bash
mkdir -p out/build
cmake -S . -B out/build -DCMAKE_BUILD_TYPE=Release
cmake --build out/build --parallel
```

## 🚀 Usage

### General arguments

| Argument | Description |
|---|---|
| `--instance <path>` | Path to the instance file |
| `--output <path>` | Path to the output file |
| `--algorithm <name>` | Algorithm to run: `ils`, `vns`, `ts`, `sa` |
| `--seed <int>` | Random seed |
| `--time <int>` | Time limit in seconds |
| `--report <int>` | Reporting interval in seconds |

### Algorithm-specific parameters

#### ILS
| Argument | Description |
|---|---|
| `--num-closest <int>` | Number of nearest neighbors to consider in the perturbation step |
| `--max-iterations <int>` | Number of iterations without improvement before restarting from the next solution in the pool |

#### VNS
| Argument | Description |
|---|---|
| `--k <int>` | Maximum shaking neighborhood size |

#### TS
| Argument | Description |
|---|---|
| `--diversify-after <int>` | Iterations without improvement before triggering diversification |

#### SA
| Argument | Description |
|---|---|
| `--initial-temperature <float>` | Starting temperature for the cooling schedule |
| `--alpha <float>` | Geometric cooling rate |
| `--max-iterations-freezed <int>` | Iterations at minimum temperature before reheating |

### Example

```bash
./out/bin/wfcrp \
  --instance instances/example_instance.instance \
  --output results/example_instance.yml \
  --algorithm ils \
  --num-closest 7 \
  --max-iterations 8725 \
  --seed 16410 \
  --time 600 \
  --report 60
```

## 📋 Instance File Format

```
<name> <n> <m> <c>
<substation_x> <substation_y>
<turbine_1_x> <turbine_1_y>
...
<turbine_n_x> <turbine_n_y>
<cable_1_cap> <cable_1_cost_per_meter>
...
<cable_m_cap> <cable_m_cost_per_meter>
```

Where `n` = number of turbines, `m` = number of cable types, `c` = maximum substation connections.

### Example (substation + first 5 turbines)

```
example_instance 50 3 6
745004.0 6110965.0
744149.0 6108859.0
743069.0 6108409.0
742169.0 6107959.0
741269.0 6107599.0
743789.0 6107599.0
...
5 430
7 480
12 610
```

## 📤 Output Format

Each run writes a structured YAML-like report to the output file (`--output`). The report has three parts:

**Header** — written once at the start, includes instance metadata, algorithm name and parameters, and the benchmark cost (cost of the best Sweep solution, used as a reference).

**Status** — a list of snapshots written every `--report` seconds during the run. Each snapshot contains the elapsed time, iteration count, current best cost, and improvement over the benchmark.

**Results** — written once at the end, with the final best cost, improvement, total iterations, elapsed time, and the full solution as a list of cable connections `(from, to, cable_type)`.

```yaml
# BEGIN REPORT
instance:
  name: example_instance
  number_of_turbines: 50
  substation_max_cable_connections: 6
benchmark:
  name: best_sweep
  cost: 37970300
algorithm:
  name: ILS
  seed: 16410
  running_time_seconds: 600
  report_period_seconds: 60
  num_closest: 7
  iterations_without_improvement: 8725
status:
  - elapsed_seconds: 60
    iterations: 6943
    best_cost: 37022710
    improvement: -2.4956%
  - elapsed_seconds: 120
    iterations: 13715
    best_cost: 37022710
    improvement: -2.4956%
  - elapsed_seconds: 180
    iterations: 20139
    best_cost: 36883600
    improvement: -2.8620%
  # ... one entry per --report interval ...
results:
  elapsed_seconds: 600
  iterations: 68540
  best_cost: 36883600
  improvement: -2.8620%
  connections:
    - { from: 1, to: 0, cable: 1 }
    - { from: 2, to: 0, cable: 3 }
    - { from: 3, to: 0, cable: 3 }
    - { from: 4, to: 3, cable: 3 }
    - { from: 5, to: 1, cable: 1 }
    # ... one entry per turbine ...
# END REPORT
```

## 👥 Authors

| Name | Affiliation | Contact |
|------|-------------|---------|
| Pedro M. de Andrade | Federal University of Bahia (UFBA), Institute of Computing | pedroma@ufba.br |
| Islame F. C. Fernandes | Federal University of Bahia (UFBA), Institute of Computing | islame.felipe@ufba.br |

## 📧 Contact

For questions, contact the lead authors: pedroma@ufba.br and islame.felipe@ufba.br.
