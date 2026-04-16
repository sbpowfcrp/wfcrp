#include "ts.hpp"
#include "../sweep.hpp"

void metaheuristics::TS::Prepare()
{
    solution_ = GetBestSweep(parameters_.instance_handler);
    individual_ = solution_;
    diversify_after_cnt_ = 0;

    tabulist_.Reset();
    UpdateTenure();
}

void metaheuristics::TS::Iterate()
{
    long long best_cost = 1e18;
    InstanceHandler::Edge best_move{0, 0};
    bool move_found = false;

    Solution individual_saved_{individual_};

    for (size_t i_node_from = 1; i_node_from < individual_.GetInstanceHandler().GetInstance().GetNumNodes(); i_node_from++)
    {
        size_t current_parent = individual_.GetParentOf(i_node_from);
        InstanceHandler::Edge edge_being_removed{i_node_from, current_parent};

        bool is_tabu = tabulist_.IsTabu(edge_being_removed);

        for (size_t i_node_to = 0; i_node_to < individual_.GetInstanceHandler().GetInstance().GetNumNodes(); i_node_to++)
        {
            InstanceHandler::Edge move{i_node_from, i_node_to};

            if (!individual_.IsMoveAllowed(move)) continue;
            if (current_parent == i_node_to) continue;

            individual_.Move(move);
            long long current_cost = individual_.GetCost();

            if (current_cost < solution_.GetCost() || (!is_tabu && current_cost < best_cost))
            {
                best_cost = current_cost;
                best_move = move;
                move_found = true;
            }

            individual_ = individual_saved_;
        }
    }

    if (move_found)
    {
        individual_.Move(best_move);
        tabulist_.RemoveTabu(best_move);
        tabulist_.AddTabu(best_move);
    }

    if (individual_.GetCost() < solution_.GetCost())
    {
        solution_ = individual_;
        diversify_after_cnt_ = 0; 
    }
    else
    {
        diversify_after_cnt_++;
    }

    if (diversify_after_cnt_ > ts_parameters_.diversify_after)
    {
        diversify_after_cnt_ = 0;

        if (Random_A_B(0, 1) < 0.5) 
        {
            UpdateTenure();
        }
        else 
        {
            tabulist_.Reset();
        }
    }
}

void metaheuristics::TS::ReportHeaderAdditional() const
{
    parameters_.output << "  diversify_after: " << ts_parameters_.diversify_after << std::endl;
}
