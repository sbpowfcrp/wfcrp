#include <map>
#include <numeric>

#include "../sweep.hpp"
#include "../utils.hpp"
#include "ils.hpp"

void metaheuristics::ILS::Prepare()
{
    // Get all different sweeps and sort them by cost.
    std::map<std::vector<size_t>, long long> parents_cost_map;
    std::vector<std::pair<long long, std::vector<size_t>>> cost_parents_vector;

    size_t num_turbs = solution_.GetInstanceHandler().GetInstance().GetNumTurbs();
    size_t max_subst_conn = solution_.GetInstanceHandler().GetInstance().GetMaxSubstConn();
    int max_cable_cap = solution_.GetInstanceHandler().GetMaxCableCap();

    for (size_t start_i = 0; start_i < num_turbs; start_i++)
    {
        for (bool is_clockwise : {false, true})
        {
            for (size_t turbs_per_group = num_turbs/max_subst_conn; turbs_per_group <= (size_t)max_cable_cap; turbs_per_group++)
            {
                Solution curr_solution = GetSweep(solution_.GetInstanceHandler(), {start_i, is_clockwise, turbs_per_group});

                if (curr_solution.GetCostParams().num_subst_conn > (int)max_subst_conn) continue;

                parents_cost_map.insert({curr_solution.GetIParents(), curr_solution.GetCost()});
            }
        }
    }
    cost_parents_vector.reserve(parents_cost_map.size());
    sweeps_i_parents_.reserve(parents_cost_map.size());

    for (auto &[parents, cost] : parents_cost_map)
    {
        cost_parents_vector.push_back({cost, parents});
    }

    std::sort(cost_parents_vector.begin(), cost_parents_vector.end());

    for (auto &[_, parents] : cost_parents_vector)
    {
        sweeps_i_parents_.push_back(parents);
    }

    current_sweep_solution_ = Solution(solution_.GetInstanceHandler(), sweeps_i_parents_[0]);
    solution_ = current_sweep_solution_;
    individual_ = current_sweep_solution_;

    current_i_sweep_ = 0;
    without_improvement_cnt_ = 0;
}

void metaheuristics::ILS::Iterate()
{
    if (without_improvement_cnt_ > ils_parameters_.iterations_without_improvement)
    {
        without_improvement_cnt_ = 0;

        current_i_sweep_++;
        current_i_sweep_ %= sweeps_i_parents_.size();

        current_sweep_solution_ = Solution(individual_.GetInstanceHandler(), sweeps_i_parents_[current_i_sweep_]);
        individual_ = current_sweep_solution_;
    }

    size_t num_nodes = individual_.GetInstanceHandler().GetInstance().GetNumNodes();

    // Pick a random node.
    size_t center_node = Random_A_B(0, num_nodes);

    // Get the `num_closest` closests nodes.
    std::vector<size_t> i_nodes(num_nodes);
    std::iota(i_nodes.begin(), i_nodes.end(), 0);
    std::sort(i_nodes.begin(), i_nodes.end(), [&](size_t a, size_t b){
        return (
            individual_.GetInstanceHandler().GetLengthOfEdge({center_node, a}) <
            individual_.GetInstanceHandler().GetLengthOfEdge({center_node, b})
        );
    });

    Shuffle(i_nodes.data(), i_nodes.data() + ils_parameters_.num_closest);

    for (size_t *i_node = i_nodes.data(); i_node < i_nodes.data() + ils_parameters_.num_closest - 1; i_node++)
    {
        bool k = rand() & 1;
        InstanceHandler::Edge edge {i_node[k], i_node[!k]};
        if (individual_.IsMoveAllowed(edge))
        {
            individual_.Move(edge);
        }
    }

    while (individual_.LocalSearch());

    if (individual_.GetCost() < current_sweep_solution_.GetCost())
    {
        current_sweep_solution_ = individual_;
        without_improvement_cnt_ = 0;
    }
    else
    {
        individual_ = current_sweep_solution_;
        without_improvement_cnt_++;
    }

    if (current_sweep_solution_.GetCost() < solution_.GetCost())
    {
        solution_ = current_sweep_solution_;
    }
}

void metaheuristics::ILS::ReportHeaderAdditional() const
{
    parameters_.output << "  num_closest: " << ils_parameters_.num_closest << std::endl;
    parameters_.output << "  iterations_without_improvement: " << ils_parameters_.iterations_without_improvement << std::endl;
}
