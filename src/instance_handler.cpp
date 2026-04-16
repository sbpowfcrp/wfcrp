#include <algorithm>

#include "instance_handler.hpp"

InstanceHandler::InstanceHandler(Instance &instance) : instance_{instance} { InitAll(); }

bool InstanceHandler::IsIntersection(Edge edge_a, Edge edge_b) const
{
    const Instance::Node &A = instance_.GetNode(edge_a.i_node_from);
    const Instance::Node &B = instance_.GetNode(edge_a.i_node_to);
    const Instance::Node &C = instance_.GetNode(edge_b.i_node_from);
    const Instance::Node &D = instance_.GetNode(edge_b.i_node_to);

    auto sign = [](double x) -> int
    {
        return (x > 0) - (x < 0);
    };

    auto inter1 = [](double a, double b, double c, double d) -> bool
    {
        if (a > b) std::swap(a, b);
        if (c > d) std::swap(c, d);
        return std::max(a, c) <= std::min(b, d);
    };

    if (A.id == B.id ||
        A.id == C.id ||
        A.id == D.id ||
        B.id == C.id ||
        B.id == D.id ||
        C.id == D.id)
    {
        return false;
    }

    const Vector2D &a = A.pos;
    const Vector2D &b = B.pos;
    const Vector2D &c = C.pos;
    const Vector2D &d = D.pos;

    if (c.Cross(a, d) == 0 && c.Cross(b, d) == 0)
    {
        return inter1(a.x, b.x, c.x, d.x) && inter1(a.y, b.y, c.y, d.y);
    }

    return sign(a.Cross(b, c)) != sign(a.Cross(b, d)) && sign(c.Cross(d, a)) != sign(c.Cross(d, b));
}

void InstanceHandler::InitDistance()
{
    size_t num_nodes = instance_.GetNumNodes();

    distance_.assign(num_nodes, std::vector<long long>(num_nodes));

    for (size_t i_node = 0; i_node < num_nodes; i_node++)
    {
        distance_[i_node][i_node] = 0;
        for (size_t j_node = i_node + 1; j_node < num_nodes; j_node++)
        {
            distance_[i_node][j_node] = (long long)(instance_.GetNode(j_node).pos - instance_.GetNode(i_node).pos).Magn();
            distance_[j_node][i_node] = distance_[i_node][j_node];
        }
    }
}

void InstanceHandler::InitBestICableForPower()
{
    int num_nodes = (int)instance_.GetNumNodes();
    int num_cables = (int)instance_.GetNumCables();

    best_i_cable_for_power_.reserve(num_nodes);

    std::vector<Instance::Cable> cables{instance_.GetCables()};
    std::sort(cables.begin(), cables.end(), [](auto a, auto b) { return a.cap < b.cap; });

    max_cable_cap_ = cables.back().cap;

    int i_cable = 0;
    for (int power = 0; power <= num_nodes && i_cable < num_cables; power++)
    {
        while (i_cable < num_cables && power > cables[i_cable].cap)
        {
            i_cable++;
        }
        best_i_cable_for_power_.push_back(i_cable);
    }
}
