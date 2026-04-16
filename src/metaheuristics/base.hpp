#pragma once

#include <chrono>

#include "../solution.hpp"

namespace metaheuristics
{
    class Base
    {
    public:
        struct Parameters
        {
            const InstanceHandler &instance_handler;
            unsigned int seed;
            std::chrono::seconds running_time_seconds;
            std::chrono::seconds report_period_seconds;
            std::ostream &output;

            Parameters(
                const InstanceHandler &instance_handler,
                unsigned int seed,
                unsigned int running_time_seconds,
                unsigned int report_period_seconds,
                std::ostream &output) : instance_handler(instance_handler),
                                        seed(seed),
                                        running_time_seconds(running_time_seconds),
                                        report_period_seconds(report_period_seconds),
                                        output(output) {}
        };

        Base(Parameters parameters) : parameters_(parameters) { InitBenchmark(); }
        virtual ~Base() {};

        inline const Solution &GetSolution() const { return solution_; }

        void Run();

    protected:
        Parameters parameters_;

        // At the end of `Run()`, `solution_` should be the algorithm's best solution found.
        Solution solution_{parameters_.instance_handler};

        virtual const std::string &GetAlgorithmName() const = 0;

        virtual void Prepare() = 0;
        virtual void Iterate() = 0;

        virtual void ReportHeaderAdditional() const {}
        virtual void ReportRunningAdditional() const {}
        virtual void ReportFooterAdditional() const {}

    private:
        std::chrono::seconds elapsed_seconds_;
        std::string benchmark_name_;
        long long benchmark_cost_;
        long long iteration_cnt_;

        void ReportHeader() const;
        void ReportRunning() const;
        void ReportFooter() const;

    private:
        void InitBenchmark();
    };
} // metaheuristics
