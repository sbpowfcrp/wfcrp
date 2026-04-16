#pragma once

#include "base.hpp"

namespace metaheuristics
{
    // This ILS implementation works by picking a turbine at random as a center point to
    // select `num_closest` closests turbines from it to perform random moves between them.
    // Periodically, if the cost doesn't improve, perform the same algorithm in another sweep.
    class ILS : public Base
    {
    public:
        struct ILS_Parameters
        {
            int num_closest;
            int iterations_without_improvement;
        };

    public:
        ILS(Parameters parameters, ILS_Parameters ils_parameters) : Base(parameters), ils_parameters_(ils_parameters) {}

    private:
        static constexpr std::string kAlgorithmName{"ILS"};

        Solution individual_{solution_};
        Solution current_sweep_solution_{solution_};
        ILS_Parameters ils_parameters_;
        std::vector<std::vector<size_t>> sweeps_i_parents_;
        size_t current_i_sweep_;
        int without_improvement_cnt_;

    private:
        inline const std::string &GetAlgorithmName() const override { return kAlgorithmName; };

        void Prepare() override;
        void Iterate() override;

        void ReportHeaderAdditional() const override;
    };
} // metaheuristics
