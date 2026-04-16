#pragma once

#include <vector>

// Write the directed minimum spanning three of size `size` obtained by applying the prim algorithm
// in the nodes of `indices`, starting from `start_i_turb` into `parents` (parent of `indices[i]` is `i_parents[indices[i]]`).
void MinimumSpanningTree(
    const std::vector<size_t> &indices,
    const std::vector<std::vector<long long>> &weights,
    size_t start_i,
    size_t size,
    std::vector<size_t> &i_parents
);
