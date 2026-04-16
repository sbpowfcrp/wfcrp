#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <format>

#include "metaheuristics/ils.hpp"
#include "metaheuristics/sa.hpp"
#include "metaheuristics/ts.hpp"
#include "metaheuristics/vns.hpp"
#include "instance.hpp"
#include "instance_handler.hpp"

namespace mh = metaheuristics;

int main(int argc, char *argv[])
{
    try
    {
        std::map<std::string, std::string> params;
        for (int i_arg = 1; i_arg < argc; i_arg++)
        {
            std::string arg = argv[i_arg];
            if (arg.substr(0, 2) == "--")
            {
                std::string key = arg.substr(2);
                std::string value = "";
                if (i_arg + 1 < argc && std::string(argv[i_arg + 1]).substr(0, 2) != "--")
                {
                    value = argv[i_arg + 1];
                    i_arg++;
                }
                params[key] = value;
            }
        }

        for (const std::string& param : {"instance", "output", "algorithm", "seed", "time", "report"})
        {
            if (!params.contains(param))
                throw std::invalid_argument(std::format("Parameter '{}' missing.", param));
        }

        std::string instance_path = params["instance"];
        std::string output_path = params["output"];
        std::string algorithm = params["algorithm"];
        unsigned int seed = std::stoul(params["seed"]);
        unsigned int time = std::stoul(params["time"]);
        unsigned int report = std::stoul(params["report"]);

        if (std::ifstream(output_path).is_open())
        {
            throw std::runtime_error(std::format("Output file '{}' already exists.", output_path));
        }

        std::ofstream outstream(output_path);
        if (!outstream)
        {
            throw std::runtime_error(std::format("Failed to open file '{}' for writing.", output_path));
        }

        Instance I(instance_path);
        InstanceHandler IH(I);

        mh::Base::Parameters base_params(IH, seed, time, report, outstream);
        std::unique_ptr<mh::Base> metaheuristic;

        if (algorithm == "ils")
        {
            for (const std::string& param : {"num-closest", "max-iterations"})
                if (!params.contains(param)) throw std::invalid_argument(std::format("Parameter '{}' missing.", param));

            mh::ILS::ILS_Parameters ils_parameters;
            ils_parameters.num_closest = std::stoi(params["num-closest"]);
            ils_parameters.iterations_without_improvement = std::stoi(params["max-iterations"]);

            metaheuristic = std::make_unique<mh::ILS>(base_params, ils_parameters);
        }
        else if (algorithm == "sa")
        {
            for (const std::string& param : {"initial-temperature", "alpha", "max-iterations-freezed"})
                if (!params.contains(param)) throw std::invalid_argument(std::format("Parameter '{}' missing.", param));

            mh::SA::SA_Parameters sa_parameters;
            sa_parameters.initial_temperature = std::stod(params["initial-temperature"]);
            sa_parameters.alpha = std::stod(params["alpha"]);
            sa_parameters.max_iterations_freezed = std::stoi(params["max-iterations-freezed"]);

            metaheuristic = std::make_unique<mh::SA>(base_params, sa_parameters);
        }
        else if (algorithm == "ts")
        {
            if (!params.contains("diversify-after")) throw std::invalid_argument("Parameter 'diversify-after' missing.");

            mh::TS::TS_Parameters ts_parameters;
            ts_parameters.diversify_after = std::stoi(params["diversify-after"]);

            metaheuristic = std::make_unique<mh::TS>(base_params, ts_parameters);
        }
        else if (algorithm == "vns")
        {
            if (!params.contains("k")) throw std::invalid_argument("Parameter 'k' missing.");

            mh::VNS::VNS_Parameters vns_params;
            vns_params.k = std::stoi(params["k"]);

            metaheuristic = std::make_unique<mh::VNS>(base_params, vns_params);
        }
        else
        {
            throw std::invalid_argument(std::format("Unknown algorithm '{}'. Use: ils, sa, ts, vns.", algorithm));
        }

        metaheuristic->Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
