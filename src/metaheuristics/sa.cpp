#include <numeric>

#include "sa.hpp"
#include "../sweep.hpp"
#include "../utils.hpp"
#include "../mst.hpp"

void metaheuristics::SA::Prepare()
{
    solution_ = GetBestSweep(parameters_.instance_handler);
    individual_ = solution_;

    const auto &instance_handler = parameters_.instance_handler;
    size_t num_nodes = instance_handler.GetInstance().GetNumNodes();

    std::vector<size_t> indices(num_nodes);
    std::iota(indices.begin(), indices.end(), 0);
    std::vector<size_t> parents(num_nodes);

    MinimumSpanningTree(indices, instance_handler.GetDistanceMatrix(), 0, num_nodes, parents);

    K_ = 1.0 / Solution(instance_handler, parents).GetCostNonPenalized();

    current_temperature_ = sa_parameters_.initial_temperature;
    last_cost_ = individual_.GetCost();
    iterations_freezed_cnt_ = 0;
    iterations_without_cooling_ = 2 * instance_handler.GetInstance().GetNumTurbs();
}

void metaheuristics::SA::Iterate()
{
    size_t i_node_from = (size_t)Random_A_B(1, individual_.GetInstanceHandler().GetInstance().GetNumNodes());
    size_t i_node_to{};
    do i_node_to = (size_t)Random_A_B(0, individual_.GetInstanceHandler().GetInstance().GetNumNodes());
    while (!individual_.IsMoveAllowed({i_node_from, i_node_to}));

    Solution individual_saved_{individual_};

    long long delta_cost = individual_.GetCost();
    individual_.Move({i_node_from, i_node_to});
    delta_cost = individual_.GetCost() - delta_cost;

    if (delta_cost > 0 && Random_0_1() > std::exp(-K_ * delta_cost / current_temperature_))
    {
        individual_ = individual_saved_;
    }

    if (iterations_without_cooling_ > 0)
    {
        iterations_without_cooling_--;
    }
    else
    {
        current_temperature_ = std::max(1e-300, current_temperature_ * sa_parameters_.alpha);
        iterations_without_cooling_ = 2 * individual_.GetInstanceHandler().GetInstance().GetNumTurbs();
    }

    long long current_cost = individual_.GetCost();

    if (current_cost < solution_.GetCost())
    {
        solution_ = individual_;
    }

    if (current_cost == last_cost_)
    {
        iterations_freezed_cnt_++;
    }
    else
    {
        iterations_freezed_cnt_ = 0;
    }

    if (iterations_freezed_cnt_ >= sa_parameters_.max_iterations_freezed)
    {
        iterations_freezed_cnt_ = 0;
        current_temperature_ = sa_parameters_.initial_temperature;
        iterations_without_cooling_ = 2 * individual_.GetInstanceHandler().GetInstance().GetNumTurbs();
    }

    last_cost_ = current_cost;
}

void metaheuristics::SA::ReportHeaderAdditional() const
{
    parameters_.output << "  initial_temperature: " << sa_parameters_.initial_temperature << std::endl;
    parameters_.output << "  alpha: " << sa_parameters_.alpha << std::endl;
    parameters_.output << "  max_iterations_freezed: " << sa_parameters_.max_iterations_freezed << std::endl;
}
