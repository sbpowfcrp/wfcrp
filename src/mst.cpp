#include <set>

#include "mst.hpp"

void MinimumSpanningTree(
    const std::vector<size_t> &indices,
    const std::vector<std::vector<long long>> &weights,
    size_t start_i,
    size_t size,
    std::vector<size_t> &i_parents
) {
    std::vector<size_t> mst{start_i};

    std::set<size_t> fringe{indices.begin(), indices.end()};
    fringe.erase(start_i);

    while (fringe.size() > 0 && mst.size() < size)
    {
        size_t next_m{};
        size_t next_f{};
        long long min_dist = 1e18;

        for (size_t m : mst)
        {
            for (size_t f : fringe)
            {
                long long dist = weights[m][f];
                if (dist < min_dist)
                {
                    next_m = m;
                    next_f = f;
                    min_dist = dist;
                }
            }
        }

        fringe.erase(next_f);
        mst.push_back(next_f);

        i_parents[next_f] = next_m;
    }
}
