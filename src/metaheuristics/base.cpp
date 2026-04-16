#include "../sweep.hpp"
#include "base.hpp"

static inline std::string sGetFormattedImprovement(long long cost, long long benchmark_cost)
{
    return std::format("{:.4f}%", ((double)cost / benchmark_cost - 1) * 100);
}

void metaheuristics::Base::Run()
{
    srand(parameters_.seed);

    parameters_.output << "# BEGIN REPORT" << std::endl;

    ReportHeader();
    ReportHeaderAdditional();

    parameters_.output << "status:" << std::endl;

    const auto start_time = std::chrono::steady_clock::now();
    auto next_output_time = start_time + parameters_.report_period_seconds;

    iteration_cnt_ = 0;

    Prepare();

    while (true)
    {
        Iterate();
        iteration_cnt_++;

        auto now = std::chrono::steady_clock::now();
        elapsed_seconds_ = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);

        if (now >= next_output_time)
        {
            ReportRunning();
            ReportRunningAdditional();
            next_output_time += parameters_.report_period_seconds;
        }

        if (elapsed_seconds_ >= parameters_.running_time_seconds)
        {
            break;
        }
    }

    ReportFooter();
    ReportFooterAdditional();

    parameters_.output << "# END REPORT" << std::endl;
}

void metaheuristics::Base::ReportHeader() const
{
    const Instance &instance = parameters_.instance_handler.GetInstance();

    parameters_.output << "instance:" << std::endl;
    parameters_.output << "  name: " << instance.GetName() << std::endl;
    parameters_.output << "  number_of_turbines: " << instance.GetNumTurbs() << std::endl;
    parameters_.output << "  substation_max_cable_connections: " << instance.GetMaxSubstConn() << std::endl;
    parameters_.output << "benchmark:" << std::endl;
    parameters_.output << "  name: " << benchmark_name_ << std::endl;
    parameters_.output << "  cost: " << benchmark_cost_ << std::endl;
    parameters_.output << "algorithm:" << std::endl;
    parameters_.output << "  name: " << GetAlgorithmName() << std::endl;
    parameters_.output << "  seed: " << parameters_.seed << std::endl;
    parameters_.output << "  running_time_seconds: " << parameters_.running_time_seconds.count() << std::endl;
    parameters_.output << "  report_period_seconds: " << parameters_.report_period_seconds.count() << std::endl;
}

void metaheuristics::Base::ReportFooter() const
{
    parameters_.output << "results:" << std::endl;
    parameters_.output << "  elapsed_seconds: " << elapsed_seconds_.count() << std::endl;
    parameters_.output << "  iterations: " << iteration_cnt_ << std::endl;
    parameters_.output << "  best_cost: " <<  solution_.GetCost() << std::endl;
    parameters_.output << "  improvement: " << sGetFormattedImprovement(solution_.GetCost(), benchmark_cost_) << std::endl;
    parameters_.output << "  connections:" << std::endl;
    for (Solution::Connection &connection : solution_.GetConnections())
    {
        const int &from = connection.node_from.id;
        const int &to = connection.node_to.id;
        const int &cable = connection.cable.id;

        parameters_.output << "    - { from: " << from << ", to: " << to << ", cable: " << cable << " }" << std::endl;
    }
}

void metaheuristics::Base::ReportRunning() const
{
    parameters_.output << "  - elapsed_seconds: " << elapsed_seconds_.count() << std::endl;
    parameters_.output << "    iterations: " << iteration_cnt_ << std::endl;
    parameters_.output << "    best_cost: " << solution_.GetCost() << std::endl;
    parameters_.output << "    improvement: " << sGetFormattedImprovement(solution_.GetCost(), benchmark_cost_) << std::endl;
}

void metaheuristics::Base::InitBenchmark()
{
    benchmark_name_ = "best_sweep";
    benchmark_cost_ = GetBestSweep(parameters_.instance_handler).GetCost();
}
