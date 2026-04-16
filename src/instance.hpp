#pragma once

#include <string>
#include <vector>

#include "vector2d.hpp"

// Stores the data of the WFCRP instance.
class Instance
{
public:
    struct Node
    {
        int id;
        Vector2D pos; // Position.
    };

    struct Cable
    {
        int id;
        int cap; // Capacity.
        int cpm; // Cost per meter.
    };

public:
    Instance(std::string filename);

    inline const std::string        &GetName()   const { return name_; }
    inline const std::vector<Node>  &GetNodes()  const { return nodes_; }
    inline const std::vector<Cable> &GetCables() const { return cables_; }
    inline const Vector2D           &GetOffset() const { return offset_; }

    inline int GetMaxSubstConn() const { return max_subst_conn_; }

    inline const Node  &GetNode(size_t i_node)   const { return nodes_[i_node]; }
    inline const Cable &GetCable(size_t i_cable) const { return cables_[i_cable]; }

    inline size_t GetNumNodes()  const { return nodes_.size(); }
    inline size_t GetNumTurbs()  const { return nodes_.size() - 1; }
    inline size_t GetNumCables() const { return cables_.size(); }

private:
    std::string        name_;
    std::vector<Node>  nodes_;
    std::vector<Cable> cables_;

    // This is the substation's original position.
    // We offset all nodes positions by it to make the substation located at (0, 0).
    Vector2D offset_;

    // Maximum number of cable connections permitted at the substation.
    int max_subst_conn_;

private:
    void InitOffset();
};
