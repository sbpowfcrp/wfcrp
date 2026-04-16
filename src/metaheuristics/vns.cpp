#include "vns.hpp"
#include "../sweep.hpp"
#include "../utils.hpp"

void metaheuristics::VNS::Prepare()
{
    solution_ = GetBestSweep(parameters_.instance_handler);
    individual_ = solution_;
    current_k_ = 1;
}

void metaheuristics::VNS::Iterate()
{
    if (current_k_ > vns_parameters_.k) current_k_ = 1;

    // Shaking
    for (int i = 0; i < current_k_; i++)
    {
        size_t i_from = (size_t)Random_A_B(1, individual_.GetInstanceHandler().GetInstance().GetNumNodes());
        size_t i_to{};
        do
        {
            i_to = (size_t)Random_A_B(0, individual_.GetInstanceHandler().GetInstance().GetNumNodes());
        } while (!individual_.IsMoveAllowed({i_from, i_to}));

        individual_.Move({i_from, i_to});
    }

    // Local Search
    while (individual_.LocalSearch());

    // Neighborhood Change
    if (individual_.GetCost() < solution_.GetCost())
    {
        solution_ = individual_;
        current_k_ = 1;
    }
    else
    {
        individual_ = solution_;
        current_k_++;
    }
}

void metaheuristics::VNS::ReportHeaderAdditional() const
{
    parameters_.output << "  k: " << vns_parameters_.k << std::endl;
}
