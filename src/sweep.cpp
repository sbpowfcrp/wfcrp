#include <algorithm>
#include <numeric>
#include <utility>

#include "mst.hpp"
#include "sweep.hpp"

static std::vector<size_t> CreateSortedITurbs(const InstanceHandler &instance_handler);

static std::vector<std::vector<size_t>> CreateSweepGroups(
    const InstanceHandler &instance_handler,
    const std::vector<size_t> &i_turbs,
    SweepArgs sweep_args
);

Solution GetSweep(const InstanceHandler &instance_handler, SweepArgs sweep_args)
{
    auto [start_i, is_clockwise, turbs_per_group] = sweep_args;

    std::vector<size_t> sorted_i_turbs = CreateSortedITurbs(instance_handler);
    std::vector<std::vector<size_t>> groups = CreateSweepGroups(instance_handler, sorted_i_turbs, {start_i, is_clockwise, turbs_per_group});

    std::vector<size_t> i_parents(instance_handler.GetInstance().GetNumNodes());

    for (std::vector<size_t> &group : groups)
    {
        MinimumSpanningTree(group, instance_handler.GetDistanceMatrix(), group.front(), group.size(), i_parents);
    }

    return Solution(instance_handler, i_parents);
}

Solution GetBestSweep(const InstanceHandler &instance_handler)
{
    size_t num_turbs = instance_handler.GetInstance().GetNumTurbs();
    size_t max_subst_conn = instance_handler.GetInstance().GetMaxSubstConn();
    int max_cable_cap = instance_handler.GetMaxCableCap();

    long long best_cost = 1e18;
    Solution best_solution = Solution(instance_handler);

    for (size_t start_i = 0; start_i < num_turbs; start_i++)
    {
        for (bool is_clockwise : {false, true})
        {
            for (size_t turbs_per_group = num_turbs/max_subst_conn; turbs_per_group <= (size_t)max_cable_cap; turbs_per_group++)
            {
                Solution curr_solution = GetSweep(instance_handler, {start_i, is_clockwise, turbs_per_group});
                long long curr_cost = curr_solution.GetCost();
                if (curr_cost < best_cost)
                {
                    best_cost = curr_cost;
                    best_solution = curr_solution;
                }
            }
        }
    }

    return best_solution;
}

// Sort nodes by clockwise order relative to the substation.
static std::vector<size_t> CreateSortedITurbs(const InstanceHandler &instance_handler)
{
    std::vector<size_t> i_turbs(instance_handler.GetInstance().GetNumTurbs());
    std::iota(i_turbs.begin(), i_turbs.end(), 1);

    std::sort(i_turbs.begin(), i_turbs.end(), [&instance_handler](size_t a, size_t b){
        double a_turb_pos_x = instance_handler.GetInstance().GetNode(a).pos.x;
        double a_turb_pos_y = instance_handler.GetInstance().GetNode(a).pos.y;
        double b_turb_pos_x = instance_handler.GetInstance().GetNode(b).pos.x;
        double b_turb_pos_y = instance_handler.GetInstance().GetNode(b).pos.y;

        return std::atan2(a_turb_pos_y, a_turb_pos_x) < std::atan2(b_turb_pos_y, b_turb_pos_x);
    });

    return i_turbs;
}

static std::vector<std::vector<size_t>> CreateSweepGroups(
    const InstanceHandler &instance_handler,
    const std::vector<size_t> &i_turbs,
    SweepArgs sweep_args
) {
    auto [start_i, is_clockwise, turbs_per_group] = sweep_args;

    size_t num_turbs = i_turbs.size();
    int curr_i_turb = start_i;

    std::vector<std::vector<size_t>> groups;

    while ((is_clockwise ? curr_i_turb < (int)num_turbs : curr_i_turb >= 0))
    {
        groups.emplace_back();
        while (groups.back().size() < turbs_per_group && (is_clockwise ? curr_i_turb < (int)num_turbs : curr_i_turb >= 0))
        {
            groups.back().push_back(i_turbs[curr_i_turb]);
            curr_i_turb += 2 * is_clockwise - 1; // (is_clockwise ? 1 : -1)
        }
    }

    curr_i_turb = (is_clockwise ? 0 : num_turbs - 1);
    while ((is_clockwise ? curr_i_turb < (int)start_i : curr_i_turb > (int)start_i))
    {
        groups.emplace_back();
        while (groups.back().size() < turbs_per_group && (is_clockwise ? curr_i_turb < (int)start_i : curr_i_turb > (int)start_i))
        {
            groups.back().push_back(i_turbs[curr_i_turb]);
            curr_i_turb += 2 * is_clockwise - 1; // (is_clockwise ? 1 : -1)
        }
    }

    std::sort(groups.begin(), groups.end(), [&is_clockwise](std::vector<size_t> &a, std::vector<size_t> &b)
              { return (is_clockwise ? a.front() < b.front() : a.back() < b.back()); });

    // Sort groups by distance to substation.
    for (std::vector<size_t> &group : groups)
    {
        std::sort(group.begin(), group.end(), [&instance_handler](size_t a, size_t b) {
            return instance_handler.GetLengthOfEdge({a, 0}) < instance_handler.GetLengthOfEdge({b, 0});
        });
    }

    return groups;
}
