#include <fstream>
#include <format>

#include "instance.hpp"

Instance::Instance(std::string filename)
{
    size_t num_nodes;
    size_t num_cables;

    std::ifstream instance_file(filename);

    if (!instance_file)
    {
        throw std::runtime_error(std::format("Failed to open file '{}'", filename));
    }

    instance_file >> name_;
    instance_file >> num_nodes;
    instance_file >> num_cables;
    instance_file >> max_subst_conn_;

    num_nodes++;

    nodes_.assign(num_nodes, Node{});
    cables_.assign(num_cables, Cable{});

    for (size_t i = 0; i < num_nodes; i++)
    {
        nodes_[i].id = i;
        instance_file >> nodes_[i].pos.x >> nodes_[i].pos.y;
    }

    for (size_t i = 0; i < num_cables; i++)
    {
        cables_[i].id = i + 1;
        instance_file >> cables_[i].cap >> cables_[i].cpm;
    }

    instance_file.close();

    InitOffset();
}

void Instance::InitOffset()
{
    offset_ = nodes_[0].pos;

    for (Node &node : nodes_)
    {
        node.pos -= offset_;
    }
}
