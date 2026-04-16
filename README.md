# An Iterated Local Search Heuristic for the Offshore Wind Farm Cable Routing Problem

This repository contains C++ implementations of metaheuristic algorithms to solve the Wind Farm Cable Routing Problem (WFCRP). The goal is to minimize the total cost of cabling connecting wind turbines to a substation, respecting capacity constraints and cable types.

## Abstract

The Wind Farm Cable Routing Problem (WFCRP) is an NP-hard optimization problem that minimizes the cost of inter-array cable infrastructure in offshore wind farms. Several metaheuristics, such as Tabu Search (TS), Simulated Annealing (SA), and Variable Neighborhood Search (VNS), have been proposed for this problem. However, these approaches often struggle to find high-quality solutions for instances with more than 100 turbines. This paper proposes an Iterated Local Search (ILS) algorithm for the WFCRP. To the best of our knowledge, this is the first application of ILS to this problem. Computational experiments compare ILS with VNS, TS, and SA on 220 benchmark instances with up to 120 turbines. The results show that ILS produces the best solutions. Statistical analysis, using the Friedman and Nemenyi tests, confirms this superiority, showing that the ILS yields statistically better results across all instance sizes.
