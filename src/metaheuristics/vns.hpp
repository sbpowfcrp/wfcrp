#pragma once

#include "base.hpp"

namespace metaheuristics
{
    class VNS : public Base
    {
    public:
        struct VNS_Parameters
        {
            int k;
        };

    public:
        VNS(Parameters parameters, VNS_Parameters vns_parameters) : Base(parameters), vns_parameters_{vns_parameters} {}

    private:
        static constexpr std::string kAlgorithmName{"VNS"};

        VNS_Parameters vns_parameters_;
        Solution individual_{solution_};
        int current_k_;

    private:
        inline const std::string &GetAlgorithmName() const override { return kAlgorithmName; };

        void Prepare() override;
        void Iterate() override;

        void ReportHeaderAdditional() const override;
    };
} // metaheuristics
