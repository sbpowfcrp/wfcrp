#include "solution.hpp"

Solution::Solution(const InstanceHandler &instance_handler, std::vector<size_t> i_parents)
    : instance_handler_{instance_handler}
{
    SetIParents(i_parents);
}

Solution::Solution(const InstanceHandler &instance_handler)
    : Solution(instance_handler, std::vector<size_t>(instance_handler.GetInstance().GetNumNodes())) { }

long long Solution::GetCost() const
{
    return (
        cost_params_.cost_cables +
        kM2 * std::max(0, cost_params_.num_subst_conn - instance_handler_.GetInstance().GetMaxSubstConn()) +
        kM3 * cost_params_.num_intersections
    );
}

long long Solution::GetCostNonPenalized() const
{
    long long cost = 0;
    for (size_t i_node = 1; i_node < instance_handler_.GetInstance().GetNumNodes(); i_node++)
    {
        int power = cost_params_.i_node_powers[i_node];
        cost += instance_handler_.GetBestCableForPower(power).cpm * instance_handler_.GetLengthOfEdge({i_node, i_parents_[i_node]});
    }
    return cost;
}


bool Solution::IsMoveAllowed(InstanceHandler::Edge edge) const
{
    if (edge.i_node_from == edge.i_node_to)
    {
        return false;
    }
    if (edge.i_node_to == 0)
    {
        return true;
    }
    return IsMoveAllowed({edge.i_node_from, i_parents_[edge.i_node_to]});
}

void Solution::Move(InstanceHandler::Edge edge)
{
    const size_t &from = edge.i_node_from;
    const size_t &to = edge.i_node_to;

    auto propagate_power = [&](size_t i_node, int power) -> void {
        while (i_node != 0)
        {
            cost_params_.cost_cables -= GetCostForCable(i_node);
            cost_params_.i_node_powers[i_node] += power;
            cost_params_.cost_cables += GetCostForCable(i_node);
            i_node = i_parents_[i_node];
        }
    };

    cost_params_.num_subst_conn -= IsConnectedToSubst(from);
    cost_params_.num_intersections -= GetAllIntersections(from);
    cost_params_.cost_cables -= GetCostForCable(from);
    propagate_power(i_parents_[from], -cost_params_.i_node_powers[from]);

    i_parents_[from] = to;

    cost_params_.num_subst_conn += IsConnectedToSubst(from);
    cost_params_.num_intersections += GetAllIntersections(from);
    cost_params_.cost_cables += GetCostForCable(from);
    propagate_power(i_parents_[from], cost_params_.i_node_powers[from]);
}

bool Solution::LocalSearch()
{
    size_t num_nodes = instance_handler_.GetInstance().GetNumNodes();

    Solution this_saved = *this;

    long long best_cost = GetCost();
    InstanceHandler::Edge best_move{0, 0};

    for (size_t i_node_from = 1; i_node_from < num_nodes; i_node_from++)
    {
        for (size_t i_node_to = 0; i_node_to < num_nodes; i_node_to++)
        {
            InstanceHandler::Edge current_move = {i_node_from, i_node_to};
            if (IsMoveAllowed(current_move))
            {
                Move(current_move);

                long long current_cost = GetCost();

                if (current_cost < best_cost)
                {
                    best_cost = current_cost;
                    best_move = current_move;
                }

                *this = this_saved;
            }
        }
    }

    if (best_move.i_node_from == 0)
    {
        return false;
    }

    Move(best_move);

    return true;
}

std::vector<Solution::Connection> Solution::GetConnections() const
{
    size_t num_turbs = instance_handler_.GetInstance().GetNumTurbs();

    std::vector<Solution::Connection> connections;
    connections.reserve(num_turbs);

    for (size_t i_turb = 1; i_turb <= num_turbs; i_turb++)
    {
        connections.push_back({
            instance_handler_.GetInstance().GetNode(i_turb),
            instance_handler_.GetInstance().GetNode(i_parents_[i_turb]),
            instance_handler_.GetBestCableForPower(cost_params_.i_node_powers[i_turb])
        });
    }

    return connections;
}

long long Solution::GetCostForCable(size_t i_node) const
{
    int power = cost_params_.i_node_powers[i_node];
    long long res;
    if (power > instance_handler_.GetMaxCableCap())
    {
        res = kM1 * (power - instance_handler_.GetMaxCableCap());
    }
    else res = instance_handler_.GetBestCableForPower(power).cpm * instance_handler_.GetLengthOfEdge({i_node, i_parents_[i_node]});

    return res;
}

void Solution::SetIParents(std::vector<size_t> i_parents)
{
    i_parents_ = i_parents;
    UpdateCostParams();
}

int Solution::GetAllIntersections(size_t i_node) const
{
    int cnt = 0;
    for (size_t j_node = 1; j_node < instance_handler_.GetInstance().GetNumNodes(); j_node++)
    {
        cnt += instance_handler_.IsIntersection({i_node, i_parents_[i_node]}, {j_node, i_parents_[j_node]});
    }
    return cnt;
}

void Solution::UpdateINodePowers()
{
    auto &i_node_powers = cost_params_.i_node_powers;

    size_t num_nodes = instance_handler_.GetInstance().GetNumNodes();

    i_node_powers.assign(num_nodes, 0);

    std::vector<bool> vis(num_nodes);

    for (size_t i_node = 1; i_node < num_nodes; i_node++)
    {
        if (vis[i_node]) continue;

        size_t j_node = i_node;
        int power = 0;
        while (j_node != 0)
        {
            power += !vis[j_node];
            i_node_powers[j_node] += power;
            vis[j_node] = true;
            j_node = i_parents_[j_node];
        }
    }
}

void Solution::UpdateCostCables()
{
    cost_params_.cost_cables = 0;

    for (size_t i_node = 1; i_node < instance_handler_.GetInstance().GetNumNodes(); i_node++)
    {
        cost_params_.cost_cables += GetCostForCable(i_node);
    }
}

void Solution::UpdateNumSusbtConn()
{
    cost_params_.num_subst_conn = 0;

    for (size_t i = 1; i < instance_handler_.GetInstance().GetNumNodes(); i++)
    {
        cost_params_.num_subst_conn += IsConnectedToSubst(i);
    }
}

void Solution::UpdateNumIntersections()
{
    size_t num_nodes = instance_handler_.GetInstance().GetNumNodes();

    cost_params_.num_intersections = 0;

    for (size_t i = 1; i < num_nodes; i++)
    {
        for (size_t j = i + 1; j < num_nodes; j++)
        {
            cost_params_.num_intersections += instance_handler_.IsIntersection({i, i_parents_[i]}, {j, i_parents_[j]});
        }
    }
}
