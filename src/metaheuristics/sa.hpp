#pragma once

#include "base.hpp"

namespace metaheuristics
{
    class SA : public Base
    {
    public:
        struct SA_Parameters
        {
            double initial_temperature;
            double alpha;
            int max_iterations_freezed;
        };

    public:
        SA(Parameters parameters, SA_Parameters sa_parameters) : Base(parameters), sa_parameters_{sa_parameters} {}

    private:
        static constexpr std::string kAlgorithmName{"SA"};

        SA_Parameters sa_parameters_;
        Solution individual_{solution_};
        double K_;
        double current_temperature_;
        long long last_cost_;
        int iterations_freezed_cnt_;
        int iterations_without_cooling_;

    private:
        inline const std::string &GetAlgorithmName() const override { return kAlgorithmName; };

        void Prepare() override;
        void Iterate() override;

        void ReportHeaderAdditional() const override;
    };
} // metaheuristics
