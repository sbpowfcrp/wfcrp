#pragma once

#include "solution.hpp"

struct SweepArgs
{
    size_t start_i;
    bool is_clockwise;
    size_t turbs_per_group;
};

Solution GetSweep(const InstanceHandler &instance_handler, SweepArgs sweep_args);

Solution GetBestSweep(const InstanceHandler &instance_handler);
