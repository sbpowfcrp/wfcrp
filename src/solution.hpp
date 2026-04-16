#pragma once

#include "instance_handler.hpp"

class Solution
{
public:
    static constexpr long long kM1 = 1e9; // Penalty cost for each power unit exceeding the cable capacity.
    static constexpr long long kM2 = 1e9; // Penalty cost for each cable connection that exceeds the substation capacity.
    static constexpr long long kM3 = 1e9; // Penalty cost for each pair of cable intersections.

public:
    struct CostParams
    {
        std::vector<int> i_node_powers;
        long long cost_cables;
        int num_subst_conn;
        int num_intersections;
    };

    struct Connection
    {
        const Instance::Node &node_from;
        const Instance::Node &node_to;
        const Instance::Cable &cable;
    };

public:
    Solution &operator=(const Solution &other)
    {
        i_parents_ = other.i_parents_;
        cost_params_ = other.cost_params_;

        return *this;
    }

public:
    Solution(const InstanceHandler &instance_handler, std::vector<size_t> i_parents);
    Solution(const InstanceHandler &instance_handler);
    Solution(const Solution &solution) : instance_handler_(solution.instance_handler_) { *this = solution; }

    inline const InstanceHandler     &GetInstanceHandler() const { return instance_handler_; }
    inline const std::vector<size_t> &GetIParents()        const { return i_parents_; }
    inline const CostParams          &GetCostParams()      const { return cost_params_; }

    inline size_t GetParentOf(size_t i_node) const { return i_parents_[i_node]; }

    long long GetCost() const;
    long long GetCostNonPenalized() const;

    // Check if `move(edge.from, edge.to)` is allowed, i.e., if this move preserves the tree structure of the solution.
    bool IsMoveAllowed(InstanceHandler::Edge edge) const;

    // Change the parent node of `edge.i_node_from` to `edge.i_node_to`.
    void Move(InstanceHandler::Edge edge);

    // Of all possible moves, if there is one that reduces the solution's cost, execute the move
    // that minimizes the cost the most and return `true`; otherwise, return `false`.
    bool LocalSearch();

    std::vector<Connection> GetConnections() const;

private:
    const InstanceHandler &instance_handler_;
    std::vector<size_t>    i_parents_;
    CostParams             cost_params_;

private:
    inline bool IsConnectedToSubst(size_t i_node) const { return i_parents_[i_node] == 0; }

    long long GetCostForCable(size_t i_node) const;

    void SetIParents(std::vector<size_t> i_parents);

    int GetAllIntersections(size_t i_node) const;

private:
    inline void UpdateCostParams()
    {
        UpdateINodePowers();
        UpdateCostCables();
        UpdateNumSusbtConn();
        UpdateNumIntersections();
    }

    void UpdateINodePowers();
    void UpdateCostCables();
    void UpdateNumSusbtConn();
    void UpdateNumIntersections();
};
