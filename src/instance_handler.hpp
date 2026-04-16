#pragma once

#include "instance.hpp"

// Compute and provide instance data.
class InstanceHandler
{
public:
    struct Edge
    {
        size_t i_node_from;
        size_t i_node_to;

        inline constexpr bool operator==(const Edge &other) const { return i_node_from == other.i_node_from && i_node_to == other.i_node_to; }
    };

public:
    InstanceHandler(Instance &instance);

    inline const Instance                            &GetInstance()                   const { return instance_; }
    inline const std::vector<std::vector<long long>> &GetDistanceMatrix()             const { return distance_; }
    inline const Instance::Cable                     &GetBestCableForPower(int power) const { return instance_.GetCable(best_i_cable_for_power_[power]); };

    inline long long GetLengthOfEdge(Edge edge) const { return distance_[edge.i_node_from][edge.i_node_to]; };
    inline int       GetMaxCableCap()           const { return max_cable_cap_; }

    bool IsIntersection(Edge edge_a, Edge edge_b) const;

private:
    const Instance &instance_;
    std::vector<std::vector<long long>>  distance_; // Nodes distance matrix.

    std::vector<size_t> best_i_cable_for_power_; // Aux vector to implement `GetBestCableForPower(int power)`

    // The maximum capacity of all cables.
    // Initialized at `InitBestICableForPower()`.
    int max_cable_cap_;

private:
    inline void InitAll()
    {
        InitDistance();
        InitBestICableForPower();
    };

    void InitDistance();
    void InitBestICableForPower();
};
